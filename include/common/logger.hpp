#pragma once

#include <mutex>
#include <string_view>

namespace common {

enum class LogLevel { INFO, WARNING, ERROR, DEBUG };

class Logger {
public:
    // TODO(Developer): Implement thread-safe logging mechanisms.
    //
    // Responsibilities:
    // - Prevent character interleaving by synchronizing writes via mutex_.
    // - Format output with details: [Timestamp] [LogLevel] [ThreadId] - Message
    // - Select standard stream (std::cout for INFO/DEBUG, std::cerr for WARNING/ERROR).
    static void log(LogLevel level, std::string_view message);

private:
    static std::mutex mutex_;
};

}  // namespace common
