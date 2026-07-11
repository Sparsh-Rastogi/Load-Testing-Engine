#include "coordinator/coordinator.hpp"
#include "common/logger.hpp"
#include <iostream>

int main() {
    common::Logger::log(common::LogLevel::INFO, "Starting Coordinator process...");

    common::CoordinatorConfig config;
    config.port = 8080;  // Placeholder port

    coordinator::Coordinator coord(config);
    coord.start();

    // Hold main thread or process commands
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();

    coord.stop();
    return 0;
}
