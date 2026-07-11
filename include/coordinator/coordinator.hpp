#pragma once

#include "common/config.hpp"
#include <string>
#include <vector>

namespace coordinator {

class Coordinator {
public:
    explicit Coordinator(const common::CoordinatorConfig& config);

    // TODO(Developer): Implement coordinator functions.
    // - Register workers.
    // - Start/stop load tests.
    // - Aggregate metrics from workers.
    // - Print aggregated benchmark summaries.
    void start();
    void stop();

private:
    common::CoordinatorConfig config_;
};

}  // namespace coordinator
