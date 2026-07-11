#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

namespace common {

class ThreadPool {
public:
    explicit ThreadPool(size_t threads);
    ~ThreadPool();

    // Prevent copy and move
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    // TODO(Developer): Implement thread pool orchestration in the source file.
    //
    // Responsibilities (Header):
    // - Enqueue arbitrary tasks and bind their parameters.
    // - Use std::packaged_task to tie execution results to a std::future.
    //
    // Responsibilities (Source):
    // - Spawn the specified number of threads in the constructor.
    // - Loop workers to grab tasks from the queue under lock, waiting on cv_.
    // - In destructor, set stop_ to true, notify all threads, and join them.
    template <typename F, typename... Args>
    auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<std::decay_t<F>, std::decay_t<Args>...>::type> {
        using return_type =
            typename std::invoke_result<std::decay_t<F>, std::decay_t<Args>...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> res = task->get_future();
        {
            std::unique_lock<std::mutex> lock(queue_mutex_);

            if (stop_) {
                throw std::runtime_error("enqueue on stopped ThreadPool");
            }

            tasks_.emplace([task]() { (*task)(); });
        }
        cv_.notify_one();
        return res;
    }

private:
    std::vector<std::thread> workers_;
    std::queue<std::function<void()>> tasks_;

    std::mutex queue_mutex_;
    std::condition_variable cv_;
    bool stop_{false};
};

}  // namespace common
