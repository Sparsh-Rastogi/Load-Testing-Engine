#include <gtest/gtest.h>
#include "coordinator/coordinator.hpp"

TEST(CoordinatorTest, LifecycleStub) {
    common::CoordinatorConfig config;
    config.port = 9000;

    coordinator::Coordinator coord(config);
    EXPECT_NO_THROW(coord.start());
    EXPECT_NO_THROW(coord.stop());
}
