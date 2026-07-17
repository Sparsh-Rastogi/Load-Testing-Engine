#include "common/config.hpp"
#include "common/logger.hpp"
#include "worker/worker.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    common::Logger::log(common::LogLevel::INFO, "Starting Worker process...");

    std::string config_path = "worker.conf";
    if (argc > 1) {
        config_path = argv[1];
    }

    common::WorkerConfig config = common::Config::load_worker_config(config_path);

    // Fall back to defaults if not parsed/empty
    if (config.target_url.empty()) {
        common::Logger::log(common::LogLevel::WARNING,
                            "No valid config file found or parsed URL is empty. Using defaults.");
        config.coordinator_host = "127.0.0.1";
        config.coordinator_port = 8080;
        config.concurrency = 4;
        config.target_url = "http://127.0.0.1:8080/";
        config.duration_seconds = 2;  // Safe low default duration
    }

    worker::Worker wrk(config);
    wrk.run();

    return 0;
}
