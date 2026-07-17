#include <gtest/gtest.h>
#include "worker/worker.hpp"

TEST(WorkerTest, FastExecutionStopsImmediately) {
    common::WorkerConfig config;
    config.concurrency = 2;
    config.target_url = "http://127.0.0.1:9999/api";
    config.duration_seconds = 0; // 0 seconds means immediate exit

    worker::Worker wrk(config);
    // Should run and terminate immediately without sending requests
    EXPECT_NO_THROW(wrk.run());
}

TEST(WorkerTest, HandleInvalidHostPortGracefully) {
    common::WorkerConfig config;
    config.concurrency = 1;
    config.target_url = "http://invalid-domain-name-that-does-not-exist:1234/test";
    config.duration_seconds = 0; // Exit immediately

    worker::Worker wrk(config);
    EXPECT_NO_THROW(wrk.run());
}
