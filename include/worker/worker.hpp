#pragma once

#include "common/config.hpp"
#include <string>

namespace worker {

class Worker {
public:
    explicit Worker(const common::WorkerConfig& config);

    // TODO(Developer): Implement worker functions.
    // - Connect/register with the coordinator.
    // - Spawn thread pools for parallel requests.
    // - Gather metrics: response times, error counts.
    // - Report back to the coordinator.
    void run();

private:
    common::WorkerConfig config_;
};

}  // namespace worker
