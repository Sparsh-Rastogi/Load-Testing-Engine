#include "common/timer.hpp"

namespace common {

Timer::Timer() {
    start();
}

void Timer::start() {
    start_time_ = std::chrono::steady_clock::now();
}

double Timer::stop() {
    auto end_time = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end_time - start_time_;
    return elapsed.count();
}

}  // namespace common
