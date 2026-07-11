#include "common/logger.hpp"

namespace common {

std::mutex Logger::mutex_;

void Logger::log([[maybe_unused]] LogLevel level, [[maybe_unused]] std::string_view message) {
    // TODO(Developer): Implement thread-safe logging with formatted levels/timestamps.
}

}  // namespace common
