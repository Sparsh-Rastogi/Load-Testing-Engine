#pragma once

#include <chrono>

namespace common {

class Timer {
public:
    Timer();

    // Starts/restarts the timer using std::chrono::steady_clock
    void start();

    // Stops the timer and returns the elapsed duration in milliseconds
    double stop();

    // TODO(Developer): Implement monotonic high-precision measurement.
    //
    // Responsibilities:
    // - Record the start timestamp in start().
    // - Query the stop timestamp in stop() and calculate the difference.
    // - Return the result as a double in milliseconds.
    // - Tip: Use std::chrono::steady_clock to prevent drift or jumps from system clock syncs.

private:
    std::chrono::time_point<std::chrono::steady_clock> start_time_;
};

}  // namespace common
