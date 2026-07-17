#pragma once

#include "common/config.hpp"
#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace worker {

struct WorkerMetrics {
    std::atomic<size_t> total_requests{0};
    std::atomic<size_t> success_requests{0};
    std::atomic<size_t> failed_requests{0};

    std::mutex latency_mutex;
    std::vector<double> latencies;
};

class Worker {
public:
    explicit Worker(const common::WorkerConfig& config);

    // Runs the standalone load generation loop and prints metrics.
    void run();

private:
    common::WorkerConfig config_;

    // Helper to parse URL into host, port, and path
    void parse_url(const std::string& url, std::string& host, int& port, std::string& path);
};

}  // namespace worker
