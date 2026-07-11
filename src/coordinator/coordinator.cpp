#include "coordinator/coordinator.hpp"
#include <iostream>

namespace coordinator {

Coordinator::Coordinator(const common::CoordinatorConfig& config) : config_(config) {}

void Coordinator::start() {
    // TODO(Developer): Start listening for worker connections, launch tests.
    std::cout << "Coordinator started on port " << config_.port << std::endl;
}

void Coordinator::stop() {
    // TODO(Developer): Stop coordinator and signal workers.
    std::cout << "Coordinator stopped." << std::endl;
}

}  // namespace coordinator
