#include "TaskScheduler.h"

#include <algorithm>    // push_heap / pop_heap
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

// ============================================================================
// CancellableImpl
//
// Internal concrete implementation of Cancellable.  All state transitions
// are performed with compare-exchange so the object is safe to share across
// the timer thread, worker threads, and the calling thread simultaneously.
// ============================================================================

class CancellableImpl final : public Cancellable
{
public:
    ScheduleStatus Status() const override
    {
        return m_status.load(std::memory_order_acquire);
    }

    void Cancel() override
    {
        // Attempt transition from Scheduled -> Cancelled first.
        ScheduleStatus expected = ScheduleStatus::Scheduled;
        if (m_status.compare_exchange_strong(expected, ScheduleStatus::Cancelled,
                                              std::memory_order_acq_rel))
            return;

        // If the task is currently running (repeating tasks mid-execution),
        // also allow Running -> Cancelled so no further cycles are scheduled.
        expected = ScheduleStatus::Running;
        m_status.compare_exchange_strong(expected, ScheduleStatus::Cancelled,
                                          std::memory_order_acq_rel);
    }

    // ---- Internal helpers (called by the scheduler / task wrappers) --------

    /// Attempt Scheduled -> Running.
    /// Returns false if the task was cancelled before execution began.
    bool TrySetRunning()
    {
        ScheduleStatus expected = ScheduleStatus::Scheduled;
        return m_status.compare_exchange_strong(expected, ScheduleStatus::Running,
                                                std::memory_order_acq_rel);
    }

    /// Running -> Completed (one-shot tasks after normal execution).
    void SetCompleted()
    {
        ScheduleStatus expected = ScheduleStatus::Running;
        m_status.compare_exchange_strong(expected, ScheduleStatus::Completed,
                                          std::memory_order_acq_rel);
    }

    /// Running -> Scheduled (repeating tasks after each invocation).
    /// No-op if Cancel() was called during execution, preserving Cancelled.
    void ResetToScheduled()
    {
        ScheduleStatus expected = ScheduleStatus::Running;
        m_status.compare_exchange_strong(expected, ScheduleStatus::Scheduled,
                                          std::memory_order_acq_rel);
    }

private:
    std::atomic<ScheduleStatus> m_status{ScheduleStatus::Scheduled};
};

// ============================================================================
// ScheduledItem  —  one element of the min-heap
// ============================================================================

struct ScheduledItem
{
    TaskScheduler::TimePoint         nextFireTime;
    std::function<void()>            task;
    std::shared_ptr<CancellableImpl> cancellable;
    bool                             repeating = false;
    TaskScheduler::Duration          period    = {};
    OverlapPolicy                    policy    = OverlapPolicy::Skip;

    /// Comparator: returns true when *this* should sit below @p rhs in the
    /// heap (i.e. *this* fires later).  Used with std::push_heap /
    /// std::pop_heap to produce a min-heap with the earliest item at front().
    bool operator>(const ScheduledItem& rhs) const
    {
        return nextFireTime > rhs.nextFireTime;
    }
};

// ============================================================================
// TaskScheduler::Impl
// ============================================================================

struct TaskScheduler::Impl
{
    explicit Impl(std::shared_ptr<TaskRunner> r)
        : runner(std::move(r))
    {}

    // --- data ---------------------------------------------------------------

    std::shared_ptr<TaskRunner>  runner;        ///< Executes tasks when due.

    std::vector<ScheduledItem>   heap;          ///< Min-heap: earliest at [0].
    std::mutex                   mutex;
    std::condition_variable      cv;
    std::atomic<bool>            stop{false};
    std::thread                  timerThread;

    // --- heap helpers (call with mutex held) --------------------------------

    void Push(ScheduledItem item)
    {
        heap.push_back(std::move(item));
        std::push_heap(heap.begin(), heap.end(),
                       [](const ScheduledItem& a, const ScheduledItem& b)
                       { return a > b; });
    }

    ScheduledItem Pop()
    {
        std::pop_heap(heap.begin(), heap.end(),
                      [](const ScheduledItem& a, const ScheduledItem& b)
                      { return a > b; });
        ScheduledItem item = std::move(heap.back());
        heap.pop_back();
        return item;
    }

    // --- timer thread -------------------------------------------------------

    void Run()
    {
        using Clock = TaskScheduler::Clock;

        std::unique_lock<std::mutex> lock(mutex);

        for (;;)
        {
            // Phase 1: sleep while the queue is empty (or until stopped).
            cv.wait(lock, [this] { return stop.load() || !heap.empty(); });

            if (stop.load())
                break;

            // Phase 2: sleep until the earliest item's fire time.
            // A notify_one() from Push() will wake us early so we can
            // re-evaluate if a new earlier item arrived.
            cv.wait_until(lock, heap.front().nextFireTime);

            if (stop.load())
                break;

            // Phase 3: dispatch every item that is now due.
            auto now = Clock::now();
            while (!heap.empty() && heap.front().nextFireTime <= now)
            {
                ScheduledItem item = Pop();

                // Discard if already cancelled.
                if (item.cancellable->Status() == ScheduleStatus::Cancelled)
                    continue;

                if (item.repeating)
                {
                    // --- Skip policy: don't enqueue if still running ---
                    if (item.policy == OverlapPolicy::Skip &&
                        item.cancellable->Status() == ScheduleStatus::Running)
                    {
                        // Slide the fire time forward and put it back.
                        item.nextFireTime += item.period;
                        Push(std::move(item));
                        continue;
                    }

                    // Pre-enqueue the next recurrence so period accuracy is
                    // not affected by the task's execution time.
                    {
                        ScheduledItem next  = item;               // copy
                        next.nextFireTime   = item.nextFireTime + item.period;
                        Push(std::move(next));
                    }

                    // Dispatch this invocation.
                    auto c    = item.cancellable;
                    auto task = item.task;
                    runner->AddTask([c, task]()
                    {
                        if (!c->TrySetRunning())
                            return;     // cancelled between enqueue and execution
                        task();
                        c->ResetToScheduled();  // no-op if cancelled during run
                    });
                }
                else
                {
                    // One-shot: dispatch and mark complete.
                    auto c    = item.cancellable;
                    auto task = item.task;
                    runner->AddTask([c, task]()
                    {
                        if (!c->TrySetRunning())
                            return;
                        task();
                        c->SetCompleted();
                    });
                }
            }
            // Loop back to Phase 1/2 for remaining items.
        }
    }
};

// ============================================================================
// TaskScheduler — construction / destruction
// ============================================================================

TaskScheduler::TaskScheduler(std::shared_ptr<TaskRunner> runner)
    : m_impl(new Impl(std::move(runner)))
{
    m_impl->timerThread = std::thread(&Impl::Run, m_impl.get());
}

TaskScheduler::TaskScheduler()
    : m_impl(new Impl(std::make_shared<TaskRunner>(1)))
{
    m_impl->timerThread = std::thread(&Impl::Run, m_impl.get());
}

TaskScheduler::~TaskScheduler()
{
    {
        std::lock_guard<std::mutex> lock(m_impl->mutex);
        m_impl->stop.store(true);
    }
    m_impl->cv.notify_all();

    if (m_impl->timerThread.joinable())
        m_impl->timerThread.join();
}

// ============================================================================
// TaskScheduler — one-shot scheduling
// ============================================================================

std::shared_ptr<Cancellable>
TaskScheduler::ScheduleTask(Duration delay, std::function<void()> task)
{
    return ScheduleTask(Clock::now() + delay, std::move(task));
}

std::shared_ptr<Cancellable>
TaskScheduler::ScheduleTask(TimePoint when, std::function<void()> task)
{
    auto cancellable = std::make_shared<CancellableImpl>();

    ScheduledItem item;
    item.nextFireTime = when;
    item.task         = std::move(task);
    item.cancellable  = cancellable;
    item.repeating    = false;

    {
        std::lock_guard<std::mutex> lock(m_impl->mutex);
        m_impl->Push(std::move(item));
    }
    m_impl->cv.notify_one();

    return cancellable;
}

// ============================================================================
// TaskScheduler — repeating scheduling
// ============================================================================

std::shared_ptr<Cancellable>
TaskScheduler::ScheduleRepeatingTask(Duration              period,
                                      std::function<void()> task,
                                      OverlapPolicy         policy)
{
    // First execution fires after one full period.
    return ScheduleRepeatingTask(period, period, std::move(task), policy);
}

std::shared_ptr<Cancellable>
TaskScheduler::ScheduleRepeatingTask(Duration              initialDelay,
                                      Duration              period,
                                      std::function<void()> task,
                                      OverlapPolicy         policy)
{
    auto cancellable = std::make_shared<CancellableImpl>();

    ScheduledItem item;
    item.nextFireTime = Clock::now() + initialDelay;
    item.task         = std::move(task);
    item.cancellable  = cancellable;
    item.repeating    = true;
    item.period       = period;
    item.policy       = policy;

    {
        std::lock_guard<std::mutex> lock(m_impl->mutex);
        m_impl->Push(std::move(item));
    }
    m_impl->cv.notify_one();

    return cancellable;
}
