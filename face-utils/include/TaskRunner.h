#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

/// @brief Encapsulates one or more worker threads that execute queued tasks.
///
/// Tasks are added via AddTask() and dispatched to whichever thread is idle.
/// Threads block on a condition variable when the queue is empty.
///
/// With a single thread, task execution order matches submission order.
/// With multiple threads, ordering is not guaranteed.
class TaskRunner
{
public:
    /// @param threadCount  Number of worker threads to spawn. Must be >= 1.
    explicit TaskRunner(std::size_t threadCount);

    /// Signals all threads to stop, drains no further tasks, and joins.
    ~TaskRunner();

    // Non-copyable, non-movable
    TaskRunner(const TaskRunner&)            = delete;
    TaskRunner& operator=(const TaskRunner&) = delete;

    /// Enqueue a callable (lambda, std::function, free function, etc.).
    void AddTask(std::function<void()> task);

    /// Enqueue a non-const member function together with the owning object.
    /// @tparam T   Class that owns the member function.
    /// @param func Pointer-to-member-function with signature void().
    /// @param obj  Pointer to the object on which func will be called.
    template<typename T>
    void AddTask(void (T::*func)(), T* obj)
    {
        AddTask([obj, func]() { (obj->*func)(); });
    }

    /// Enqueue a const member function together with the owning object.
    template<typename T>
    void AddTask(void (T::*func)() const, const T* obj)
    {
        AddTask([obj, func]() { (obj->*func)(); });
    }

private:
    void WorkerThread();

    std::vector<std::thread>        m_threads;
    std::queue<std::function<void()>> m_taskQueue;
    std::mutex                      m_mutex;
    std::condition_variable         m_condition;
    std::atomic<bool>               m_stop;
};
