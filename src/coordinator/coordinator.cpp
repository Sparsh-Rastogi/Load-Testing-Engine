#include "coordinator/coordinator.hpp"
#include "common/logger.hpp"
#include <chrono>
#include <iostream>
#include <sstream>
#include <thread>

namespace coordinator {

Coordinator::Coordinator(const common::CoordinatorConfig& config) : config_(config) {}

void Coordinator::start() {
    running_ = true;
    if (!server_socket_.bind(config_.port)) {
        common::Logger::log(common::LogLevel::ERROR,
                            "Coordinator failed to bind to port " + std::to_string(config_.port));
        return;
    }
    if (!server_socket_.listen(10)) {
        common::Logger::log(common::LogLevel::ERROR,
                            "Coordinator failed to listen on port " + std::to_string(config_.port));
        return;
    }

    common::Logger::log(common::LogLevel::INFO,
                        "Coordinator listening on port " + std::to_string(config_.port));
    common::Logger::log(common::LogLevel::INFO, "Awaiting registration of " +
                                                    std::to_string(config_.worker_hosts.size()) +
                                                    " workers...");

    while (running_ && worker_sockets_.size() < config_.worker_hosts.size()) {
        common::Socket client = server_socket_.accept();
        if (client.fd() == -1) {
            if (!running_) {
                break;
            }
            continue;
        }

        std::string reg_msg = client.receive(1024);
        if (reg_msg.rfind("REGISTER", 0) == 0) {
            std::string worker_id = "Unknown";
            std::istringstream iss(reg_msg);
            std::string cmd;
            if (iss >> cmd >> worker_id) {
                // Extracted successfully
            }
            common::Logger::log(common::LogLevel::INFO, "Worker registered: " + worker_id);
            worker_sockets_.push_back(std::move(client));
        } else {
            client.close();
        }
    }

    if (!running_) {
        return;
    }

    if (worker_sockets_.empty()) {
        common::Logger::log(common::LogLevel::WARNING, "No workers registered. Exiting test run.");
        return;
    }

    common::Logger::log(common::LogLevel::INFO,
                        "All workers registered. Broadcasting trigger: target=" + config_.target_url);

    // Format start command
    std::string start_cmd = "START " + config_.target_url + " " +
                            std::to_string(config_.concurrency) + " " +
                            std::to_string(config_.duration_seconds) + "\n";

    for (auto& sock : worker_sockets_) {
        sock.send(start_cmd);
    }

    common::Logger::log(common::LogLevel::INFO,
                        "Test triggered. Awaiting execution completion...");

    // Wait test duration + 2s buffer for completion
    std::this_thread::sleep_for(std::chrono::seconds(config_.duration_seconds + 2));

    common::Logger::log(common::LogLevel::INFO, "Collecting performance metrics from workers...");

    size_t total_requests = 0;
    size_t success_requests = 0;
    size_t failed_requests = 0;
    double weighted_latency_sum = 0.0;
    double weighted_p95_sum = 0.0;

    for (auto& sock : worker_sockets_) {
        std::string metrics_msg = sock.receive(1024);
        if (metrics_msg.rfind("METRICS", 0) == 0) {
            std::istringstream iss(metrics_msg);
            std::string cmd;
            size_t w_total = 0, w_success = 0, w_failed = 0;
            double w_avg = 0.0, w_p95 = 0.0;

            if (iss >> cmd >> w_total >> w_success >> w_failed >> w_avg >> w_p95) {
                total_requests += w_total;
                success_requests += w_success;
                failed_requests += w_failed;
                weighted_latency_sum += w_avg * static_cast<double>(w_total);
                weighted_p95_sum += w_p95 * static_cast<double>(w_total);
            }
        }
    }

    double global_avg_latency =
        total_requests > 0 ? (weighted_latency_sum / static_cast<double>(total_requests)) : 0.0;
    double global_p95_latency =
        total_requests > 0 ? (weighted_p95_sum / static_cast<double>(total_requests)) : 0.0;
    double rps = config_.duration_seconds > 0
                     ? (static_cast<double>(total_requests) / static_cast<double>(config_.duration_seconds))
                     : 0.0;

    std::cout << "\n================= AGGREGATED BENCHMARK REPORT =================\n"
              << "Total Requests:       " << total_requests << "\n"
              << "Successful Requests:  " << success_requests << "\n"
              << "Failed Requests:      " << failed_requests << "\n"
              << "Average Latency:      " << global_avg_latency << " ms\n"
              << "P95 Latency:          " << global_p95_latency << " ms\n"
              << "Requests per Second:  " << rps << "\n"
              << "===============================================================\n"
              << std::endl;

    // Shutdown signal to workers
    for (auto& sock : worker_sockets_) {
        sock.send("STOP\n");
        sock.close();
    }
    worker_sockets_.clear();
}

void Coordinator::stop() {
    running_ = false;
    server_socket_.close();
    for (auto& sock : worker_sockets_) {
        sock.close();
    }
    worker_sockets_.clear();
}

}  // namespace coordinator
