#include "common/config.hpp"

namespace common {

WorkerConfig Config::load_worker_config([[maybe_unused]] const std::string& filepath) {
    // TODO(Developer): Load and parse worker configuration file.
    return WorkerConfig{};
}

CoordinatorConfig Config::load_coordinator_config([[maybe_unused]] const std::string& filepath) {
    // TODO(Developer): Load and parse coordinator configuration file.
    return CoordinatorConfig{};
}

}  // namespace common
