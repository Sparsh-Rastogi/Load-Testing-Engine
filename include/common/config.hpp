#pragma once

#include <string>
#include <vector>

namespace common {

struct WorkerConfig {
    std::string coordinator_host;
    int coordinator_port{0};
    int concurrency{1};
    std::string target_url;
    int duration_seconds{0};
};

struct CoordinatorConfig {
    int port{0};
    std::vector<std::string> worker_hosts;
    std::string target_url;
    int concurrency{1};
    int duration_seconds{0};
};

class Config {
public:
    // TODO(Developer): Implement configuration parsing from files.
    //
    // Responsibilities:
    // - Read configuration parameters from the specified path.
    // - Handle errors if files are missing or fields are invalid.
    // - Tip: A simple key-value file reader (e.g., config line-by-line parser)
    //   is easy to implement. Format:
    //   coordinator_host=127.0.0.1
    //   coordinator_port=8080
    //   concurrency=4
    //   target_url=http://localhost
    //   duration_seconds=10
    static WorkerConfig load_worker_config(const std::string& filepath);
    static CoordinatorConfig load_coordinator_config(const std::string& filepath);
};

}  // namespace common
