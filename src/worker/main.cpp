#include "worker/worker.hpp"
#include "common/logger.hpp"

int main() {
    common::Logger::log(common::LogLevel::INFO, "Starting Worker process...");

    common::WorkerConfig config;
    config.coordinator_host = "127.0.0.1";
    config.coordinator_port = 8080;
    config.concurrency = 4;
    config.target_url = "http://localhost";
    config.duration_seconds = 10;

    worker::Worker wrk(config);
    wrk.run();

    return 0;
}
