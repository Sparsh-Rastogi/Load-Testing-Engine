#include "worker/worker.hpp"
#include <iostream>

namespace worker {

Worker::Worker(const common::WorkerConfig& config) : config_(config) {}

void Worker::run() {
    // TODO(Developer): Run the worker logic: connect to coordinator, start thread pool, generate requests, measure latency.
    std::cout << "Worker running. Concurrency: " << config_.concurrency
              << ", target URL: " << config_.target_url << std::endl;
}

}  // namespace worker
