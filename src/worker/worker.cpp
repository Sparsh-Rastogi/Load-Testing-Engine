#include "worker/worker.hpp"
#include "common/http_client.hpp"
#include "common/logger.hpp"
#include "common/thread_pool.hpp"
#include "common/timer.hpp"
#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <numeric>
#include <vector>

namespace worker {

Worker::Worker(const common::WorkerConfig& config) : config_(config) {}

void Worker::parse_url(const std::string& url, std::string& host, int& port, std::string& path) {
    std::string temp = url;
    port = 80;  // Default HTTP port

    if (temp.rfind("http://", 0) == 0) {
        temp = temp.substr(7);
    } else if (temp.rfind("https://", 0) == 0) {
        temp = temp.substr(8);
        port = 443;
    }

    auto slash_pos = temp.find('/');
    std::string host_port_part;
    if (slash_pos == std::string::npos) {
        host_port_part = temp;
        path = "/";
    } else {
        host_port_part = temp.substr(0, slash_pos);
        path = temp.substr(slash_pos);
    }

    auto colon_pos = host_port_part.find(':');
    if (colon_pos == std::string::npos) {
        host = host_port_part;
    } else {
        host = host_port_part.substr(0, colon_pos);
        try {
            port = std::stoi(host_port_part.substr(colon_pos + 1));
        } catch (...) {
            port = 80;
        }
    }
}

void Worker::run() {
    common::Logger::log(common::LogLevel::INFO, "Connecting to Coordinator at " +
                                                    config_.coordinator_host + ":" +
                                                    std::to_string(config_.coordinator_port));

    common::Socket coord_sock;
    bool connected = false;
    for (int retries = 0; retries < 15; ++retries) {
        if (coord_sock.connect(config_.coordinator_host, config_.coordinator_port)) {
            connected = true;
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    if (!connected) {
        common::Logger::log(common::LogLevel::ERROR, "Worker failed to connect to Coordinator");
        return;
    }

    // Registration handshake
    std::string hostname = "worker-node";
    char name_buf[256];
    if (::gethostname(name_buf, sizeof(name_buf)) == 0) {
        hostname = name_buf;
    }
    std::string register_cmd = "REGISTER " + hostname + "\n";
    coord_sock.send(register_cmd);

    common::Logger::log(common::LogLevel::INFO,
                        "Registered with Coordinator. Awaiting trigger command...");

    std::string trigger = coord_sock.receive(1024);
    if (trigger.rfind("START", 0) != 0) {
        common::Logger::log(common::LogLevel::ERROR,
                            "Worker received invalid trigger command: " + trigger);
        coord_sock.close();
        return;
    }

    // Parse: START <url> <concurrency> <duration>
    std::istringstream iss(trigger);
    std::string cmd, target_url;
    int concurrency = config_.concurrency;
    int duration_seconds = config_.duration_seconds;

    if (iss >> cmd >> target_url >> concurrency >> duration_seconds) {
        // Parsed successfully
    }

    std::string host;
    int port = 80;
    std::string path;
    parse_url(target_url, host, port, path);

    common::Logger::log(
        common::LogLevel::INFO,
        "Worker executing load test on " + host + ":" + std::to_string(port) + path +
            " with concurrency=" + std::to_string(concurrency) +
            " and duration=" + std::to_string(duration_seconds) + "s");

    common::ThreadPool pool(concurrency);
    WorkerMetrics metrics;

    auto start_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::seconds(duration_seconds);

    // Define worker task thread execution loop
    auto worker_task = [&]() {
        common::HttpClient client;
        common::HttpRequest req;
        req.method = "GET";
        req.path = path;
        req.headers["Host"] = host;
        req.headers["User-Agent"] = "Load-Testing-Engine-Worker";

        while (std::chrono::steady_clock::now() - start_time < duration) {
            auto res = client.send_request(host, port, req);
            metrics.total_requests++;
            if (res.status_code >= 200 && res.status_code < 400) {
                metrics.success_requests++;
            } else {
                metrics.failed_requests++;
            }

            {
                std::lock_guard<std::mutex> lock(metrics.latency_mutex);
                metrics.latencies.push_back(res.latency_ms);
            }
        }
    };

    std::vector<std::future<void>> futures;
    for (int i = 0; i < concurrency; ++i) {
        futures.push_back(pool.enqueue(worker_task));
    }

    // Await execution completions
    for (auto& fut : futures) {
        fut.get();
    }

    // Aggregate metrics
    double p95 = 0.0;
    double avg = 0.0;
    {
        std::lock_guard<std::mutex> lock(metrics.latency_mutex);
        if (!metrics.latencies.empty()) {
            std::sort(metrics.latencies.begin(), metrics.latencies.end());
            size_t p95_idx = static_cast<size_t>(static_cast<double>(metrics.latencies.size()) * 0.95);
            p95 = metrics.latencies[p95_idx];

            double sum = std::accumulate(metrics.latencies.begin(), metrics.latencies.end(), 0.0);
            avg = sum / static_cast<double>(metrics.latencies.size());
        }
    }

    // Send final stats back to coordinator
    std::string report = "METRICS " + std::to_string(metrics.total_requests.load()) + " " +
                         std::to_string(metrics.success_requests.load()) + " " +
                         std::to_string(metrics.failed_requests.load()) + " " +
                         std::to_string(avg) + " " + std::to_string(p95) + "\n";
    coord_sock.send(report);

    common::Logger::log(common::LogLevel::INFO,
                        "Metrics reported back to Coordinator. Awaiting termination...");

    // Block on coordinator STOP command
    std::string stop_sig = coord_sock.receive(1024);
    coord_sock.close();
    common::Logger::log(common::LogLevel::INFO, "Worker shut down gracefully.");
}

}  // namespace worker
