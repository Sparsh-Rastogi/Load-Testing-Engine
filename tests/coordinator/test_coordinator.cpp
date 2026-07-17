#include <gtest/gtest.h>
#include "coordinator/coordinator.hpp"

TEST(CoordinatorTest, LifecycleEmptyWorkers) {
    common::CoordinatorConfig config;
    config.port = 9005;
    config.worker_hosts = {}; // Empty list
    config.target_url = "http://localhost/";
    config.concurrency = 2;
    config.duration_seconds = 0;

    coordinator::Coordinator coord(config);
    // Should boot up and return immediately because worker list is empty
    EXPECT_NO_THROW(coord.start());
    EXPECT_NO_THROW(coord.stop());
}
