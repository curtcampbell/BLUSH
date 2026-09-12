#include "TaskRunner.h"

#include <stdexcept>

TaskRunner::TaskRunner(std::size_t threadCount)
    : m_stop(false)
{
    if (threadCount == 0)
        throw std::invalid_argument("TaskRunner: threadCount must be >= 1");

    m_threads.reserve(threadCount);
    for (std::size_t i = 0; i < threadCount; ++i)
        m_threads.emplace_back(&TaskRunner::WorkerThread, this);
}

TaskRunner::~TaskRunner()
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_stop.store(true);
    }
    m_condition.notify_all();

    for (auto& t : m_threads)
    {
        if (t.joinable())
            t.join();
    }
}

void TaskRunner::AddTask(std::function<void()> task)
{
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_taskQueue.push(std::move(task));
    }
    m_condition.notify_one();
}

void TaskRunner::WorkerThread()
{
    for (;;)
    {
        std::function<void()> task;

        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // Sleep until there is work to do or we are told to stop.
            m_condition.wait(lock, [this]
            {
                return m_stop.load() || !m_taskQueue.empty();
            });

            if (m_stop.load() && m_taskQueue.empty())
                return;

            task = std::move(m_taskQueue.front());
            m_taskQueue.pop();
        }

        task();
    }
}
