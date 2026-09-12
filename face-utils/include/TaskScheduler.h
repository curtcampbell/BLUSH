#pragma once

#include "Cancellable.h"
#include "TaskRunner.h"

#include <chrono>
#include <functional>
#include <memory>

/// Controls behaviour when a repeating task's next period fires while
/// the previous invocation is still executing.
///
/// The default is Skip. Document the chosen policy at the call site when
/// using Queue, because concurrent invocations of the same task can cause
/// data races if the task itself is not thread-safe.
enum class OverlapPolicy
{
    /// Skip the cycle — do not enqueue a new invocation.
    /// The task's cadence resumes normally on the following period.
    /// (default)
    Skip,

    /// Always enqueue on schedule regardless of whether the prior
    /// invocation has finished. Requires the task to be thread-safe
    /// when the backing TaskRunner has more than one worker thread.
    Queue
};

/// Schedules tasks for future or periodic execution, dispatching them
/// to a TaskRunner when their time arrives.
///
/// A dedicated timer thread maintains a min-heap sorted by next scheduled
/// fire time. When a task comes due it is handed to the TaskRunner.
/// All public methods are thread-safe.
///
/// Usage
/// -----
///   // Shared runner, single scheduler
///   auto runner    = std::make_shared<TaskRunner>(4);
///   TaskScheduler  scheduler(runner);
///
///   auto handle = scheduler.ScheduleTask(
///       std::chrono::seconds(2), []{ doWork(); });
///
///   // Later...
///   handle->Cancel();
class TaskScheduler
{
public:
    using Clock     = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration  = Clock::duration;

    /// Constructs a scheduler that dispatches through an externally-owned
    /// TaskRunner.  The runner must outlive this scheduler.
    explicit TaskScheduler(std::shared_ptr<TaskRunner> runner);

    /// Constructs a scheduler with an internally-created single-threaded
    /// TaskRunner.  Use this when no external runner is needed.
    TaskScheduler();

    /// Signals the timer thread to stop and joins it.
    /// Tasks already dispatched to the TaskRunner are not interrupted.
    ~TaskScheduler();

    // Non-copyable, non-movable
    TaskScheduler(const TaskScheduler&)            = delete;
    TaskScheduler& operator=(const TaskScheduler&) = delete;

    // -----------------------------------------------------------------------
    // One-shot scheduling
    // -----------------------------------------------------------------------

    /// Enqueue @p task once after @p delay has elapsed.
    std::shared_ptr<Cancellable> ScheduleTask(
        Duration              delay,
        std::function<void()> task);

    /// Enqueue @p task once at the absolute time @p when.
    std::shared_ptr<Cancellable> ScheduleTask(
        TimePoint             when,
        std::function<void()> task);

    // -----------------------------------------------------------------------
    // Repeating scheduling
    // -----------------------------------------------------------------------

    /// Enqueue @p task repeatedly every @p period.
    /// The first execution fires after one full @p period has elapsed.
    ///
    /// @param period  Time between successive executions.
    /// @param task    Callable to invoke.
    /// @param policy  Overlap behaviour (default: OverlapPolicy::Skip).
    ///                When Skip is used and the prior invocation is still
    ///                running at the next period boundary, that cycle is
    ///                skipped and the cadence resumes at the following period.
    std::shared_ptr<Cancellable> ScheduleRepeatingTask(
        Duration              period,
        std::function<void()> task,
        OverlapPolicy         policy = OverlapPolicy::Skip);

    /// Enqueue @p task repeatedly every @p period, with a custom
    /// @p initialDelay before the very first execution.
    ///
    /// @param initialDelay  Delay before the first invocation.
    /// @param period        Time between subsequent executions.
    /// @param task          Callable to invoke.
    /// @param policy        Overlap behaviour (default: OverlapPolicy::Skip).
    std::shared_ptr<Cancellable> ScheduleRepeatingTask(
        Duration              initialDelay,
        Duration              period,
        std::function<void()> task,
        OverlapPolicy         policy = OverlapPolicy::Skip);

private:
    struct Impl;
    std::unique_ptr<Impl> m_impl;
};
