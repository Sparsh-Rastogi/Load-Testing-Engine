#include <gtest/gtest.h>
#include <future>
#include <type_traits>
#include "common/config.hpp"
#include "common/logger.hpp"
#include "common/thread_pool.hpp"
#include "common/timer.hpp"

TEST(CommonTest, LoggerStubs) {
    // Check that Logger levels are defined and callable
    EXPECT_NO_THROW(common::Logger::log(common::LogLevel::INFO, "Verification INFO log"));
    EXPECT_NO_THROW(common::Logger::log(common::LogLevel::WARNING, "Verification WARNING log"));
    EXPECT_NO_THROW(common::Logger::log(common::LogLevel::ERROR, "Verification ERROR log"));
    EXPECT_NO_THROW(common::Logger::log(common::LogLevel::DEBUG, "Verification DEBUG log"));
}

TEST(CommonTest, ConfigStructures) {
    // Verify configuration structures compile and support initializers
    common::WorkerConfig worker_config{.coordinator_host = "localhost",
                                       .coordinator_port = 8080,
                                       .concurrency = 8,
                                       .target_url = "http://127.0.0.1:80",
                                       .duration_seconds = 60};
    EXPECT_EQ(worker_config.concurrency, 8);
    EXPECT_EQ(worker_config.duration_seconds, 60);

    common::CoordinatorConfig coord_config{.port = 8080,
                                           .worker_hosts = {"192.168.1.2", "192.168.1.3"}};
    EXPECT_EQ(coord_config.port, 8080);
    EXPECT_EQ(coord_config.worker_hosts.size(), 2);
}

TEST(CommonTest, TimerMeasurementInterface) {
    common::Timer timer;
    timer.start();
    double elapsed = timer.stop();
    EXPECT_GE(elapsed, 0.0);
}

TEST(CommonTest, ThreadPoolEnqueueInterface) {
    // Instantiate ThreadPool with 0 workers to verify compiles and type resolution of enqueue.
    // The developer will implement the worker loop itself in Milestone 1 implementation.
    common::ThreadPool pool(0);

    // Verify enqueue templates compile and resolve std::future return types.
    auto fut = pool.enqueue([](int val) { return val * 2; }, 5);
    static_assert(std::is_same_v<decltype(fut), std::future<int>>);
    SUCCEED();
}
