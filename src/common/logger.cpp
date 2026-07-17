#include "common/logger.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>

namespace common {

std::mutex Logger::mutex_;

void Logger::log(LogLevel level, std::string_view message) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Get current system time with millisecond precision
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::tm tm_now{};
    // Use thread-safe POSIX localtime_r
    localtime_r(&time_t_now, &tm_now);

    // Format timestamp
    std::ostringstream time_ss;
    time_ss << std::put_time(&tm_now, "%Y-%m-%d %H:%M:%S") << '.' << std::setfill('0')
            << std::setw(3) << ms.count();

    // Map log levels to strings
    std::string_view level_str;
    switch (level) {
        case LogLevel::INFO:
            level_str = "INFO";
            break;
        case LogLevel::WARNING:
            level_str = "WARNING";
            break;
        case LogLevel::ERROR:
            level_str = "ERROR";
            break;
        case LogLevel::DEBUG:
            level_str = "DEBUG";
            break;
    }

    // Select standard output/error stream
    auto& out_stream = (level == LogLevel::WARNING || level == LogLevel::ERROR) ? std::cerr : std::cout;

    // Output formatted log line
    out_stream << "[" << time_ss.str() << "] [" << level_str << "] [Thread: "
               << std::this_thread::get_id() << "] " << message << std::endl;
}

}  // namespace common
