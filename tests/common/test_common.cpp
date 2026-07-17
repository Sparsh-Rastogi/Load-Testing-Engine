#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <future>
#include <thread>
#include <vector>
#include "common/config.hpp"
#include "common/http.hpp"
#include "common/http_client.hpp"
#include "common/logger.hpp"
#include "common/socket.hpp"
#include "common/thread_pool.hpp"
#include "common/timer.hpp"

// 1. Logger Tests
TEST(CommonTest, LoggerOutput) {
    EXPECT_NO_THROW(common::Logger::log(common::LogLevel::INFO, "Logger functional check - INFO"));
    EXPECT_NO_THROW(
        common::Logger::log(common::LogLevel::WARNING, "Logger functional check - WARNING"));
    EXPECT_NO_THROW(
        common::Logger::log(common::LogLevel::ERROR, "Logger functional check - ERROR"));
    EXPECT_NO_THROW(
        common::Logger::log(common::LogLevel::DEBUG, "Logger functional check - DEBUG"));
}

// 2. Config Parser Tests
TEST(CommonTest, WorkerConfigParsing) {
    const std::string filename = "test_worker.conf";
    std::ofstream file(filename);
    ASSERT_TRUE(file.is_open());

    file << "# Test configuration for worker\n"
         << "coordinator_host = 192.168.1.10\n"
         << "coordinator_port = 9091\n"
         << "\n"
         << "concurrency = 16\n"
         << "target_url = http://example.com/api\n"
         << "duration_seconds = 45\n";
    file.close();

    auto config = common::Config::load_worker_config(filename);

    EXPECT_EQ(config.coordinator_host, "192.168.1.10");
    EXPECT_EQ(config.coordinator_port, 9091);
    EXPECT_EQ(config.concurrency, 16);
    EXPECT_EQ(config.target_url, "http://example.com/api");
    EXPECT_EQ(config.duration_seconds, 45);

    std::remove(filename.c_str());
}

TEST(CommonTest, CoordinatorConfigParsing) {
    const std::string filename = "test_coordinator.conf";
    std::ofstream file(filename);
    ASSERT_TRUE(file.is_open());

    file << "port = 8888\n"
         << "worker_hosts = 10.0.0.1, 10.0.0.2 ,10.0.0.3\n";
    file.close();

    auto config = common::Config::load_coordinator_config(filename);

    EXPECT_EQ(config.port, 8888);
    ASSERT_EQ(config.worker_hosts.size(), 3);
    EXPECT_EQ(config.worker_hosts[0], "10.0.0.1");
    EXPECT_EQ(config.worker_hosts[1], "10.0.0.2");
    EXPECT_EQ(config.worker_hosts[2], "10.0.0.3");

    std::remove(filename.c_str());
}

// 3. Timer Tests
TEST(CommonTest, TimerMeasurement) {
    common::Timer timer;
    timer.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    double elapsed = timer.stop();

    // Since thread scheduler granularity can vary, we check >= 40ms to be safe
    EXPECT_GE(elapsed, 40.0);
    EXPECT_LE(elapsed, 500.0);  // Upper boundary check to avoid extreme anomalies
}

// 4. ThreadPool Tests
TEST(CommonTest, ThreadPoolExecution) {
    common::ThreadPool pool(4);
    std::vector<std::future<int>> futures;

    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.enqueue([i]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            return i * i;
        }));
    }

    for (int i = 0; i < 10; ++i) {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}

// 5. Socket Tests
TEST(CommonTest, SocketConnectionFailure) {
    common::Socket sock;
    // Attempting connection on an unassigned port on localhost should fail
    bool success = sock.connect("127.0.0.1", 9999);
    EXPECT_FALSE(success);
}

// 6. HTTP Tests
TEST(CommonTest, HttpRequestSerialization) {
    common::HttpRequest req;
    req.method = "POST";
    req.path = "/submit";
    req.headers["Host"] = "localhost";
    req.headers["Content-Type"] = "application/json";
    req.body = "{\"key\": \"val\"}";

    std::string expected_str =
        "POST /submit HTTP/1.1\r\n"
        "Content-Type: application/json\r\n"
        "Host: localhost\r\n"
        "Content-Length: 14\r\n"
        "\r\n"
        "{\"key\": \"val\"}";

    std::string serialized = req.serialize();
    // Since map ordering can vary, we check standard lines exist
    EXPECT_NE(serialized.find("POST /submit HTTP/1.1\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("Host: localhost\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("Content-Type: application/json\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("Content-Length: 14\r\n"), std::string::npos);
    EXPECT_NE(serialized.find("\r\n\r\n{\"key\": \"val\"}"), std::string::npos);
}

TEST(CommonTest, HttpResponseParsing) {
    std::string raw_res =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 12\r\n"
        "\r\n"
        "Hello World!";

    auto res = common::HttpResponse::parse(raw_res);
    EXPECT_EQ(res.version, "HTTP/1.1");
    EXPECT_EQ(res.status_code, 200);
    EXPECT_EQ(res.status_message, "OK");
    EXPECT_EQ(res.headers["Content-Type"], "text/plain");
    EXPECT_EQ(res.headers["Content-Length"], "12");
    EXPECT_EQ(res.body, "Hello World!");
}

// 7. HttpClient Tests
TEST(CommonTest, HttpClientFailureHandling) {
    common::HttpClient client;
    common::HttpRequest req;
    req.method = "GET";
    req.path = "/";

    // Requesting to an unassigned port on localhost should fail gracefully
    auto res = client.send_request("127.0.0.1", 9999, req);
    EXPECT_EQ(res.status_code, -1);
    EXPECT_EQ(res.status_message, "Connection Failed");
    EXPECT_GT(res.latency_ms, 0.0);
}
