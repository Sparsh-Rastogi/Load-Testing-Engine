#include "common/thread_pool.hpp"

namespace common {

ThreadPool::ThreadPool(size_t threads) {
    // TODO(Developer): Initialize threads in the pool and start task loop.
    (void)threads;
}

ThreadPool::~ThreadPool() {
    // TODO(Developer): Gracefully shut down threads.
}

void ThreadPool::enqueue([[maybe_unused]] std::function<void()> task) {
    // TODO(Developer): Enqueue task, notifying workers.
}

}  // namespace common
