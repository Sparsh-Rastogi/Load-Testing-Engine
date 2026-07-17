#include "common/config.hpp"
#include "common/logger.hpp"
#include "coordinator/coordinator.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    common::Logger::log(common::LogLevel::INFO, "Starting Coordinator process...");

    std::string config_path = "coordinator.conf";
    if (argc > 1) {
        config_path = argv[1];
    }

    common::CoordinatorConfig config = common::Config::load_coordinator_config(config_path);

    // Fallback defaults if configuration is empty
    if (config.port == 0) {
        common::Logger::log(common::LogLevel::WARNING,
                            "No valid coordinator config found. Using default parameters.");
        config.port = 8080;
        config.worker_hosts = {"127.0.0.1"};
        config.target_url = "http://127.0.0.1:80/";
        config.concurrency = 2;
        config.duration_seconds = 5;
    }

    coordinator::Coordinator coord(config);
    coord.start();

    return 0;
}
