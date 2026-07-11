#include <gtest/gtest.h>
#include "worker/worker.hpp"

TEST(WorkerTest, LifecycleStub) {
    common::WorkerConfig config;
    config.concurrency = 1;
    config.target_url = "http://127.0.0.1";

    worker::Worker wrk(config);
    EXPECT_NO_THROW(wrk.run());
}
