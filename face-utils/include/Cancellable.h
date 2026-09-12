#pragma once

/// Execution state of a scheduled task.
enum class ScheduleStatus
{
    Scheduled,  ///< Queued; has not yet started executing.
    Running,    ///< Currently executing on a worker thread.
    Completed,  ///< Finished normally (one-shot tasks only).
    Cancelled   ///< Terminated by a call to Cancel().
};

/// Handle returned by TaskScheduler that lets the caller observe or
/// cancel a scheduled (or repeating) operation.
///
/// Instances are always held through std::shared_ptr<Cancellable>.
/// The underlying task remains scheduled as long as the scheduler is
/// alive, regardless of whether the caller retains the handle.
class Cancellable
{
public:
    virtual ~Cancellable() = default;

    /// Returns the current execution status of the associated task.
    /// Safe to call from any thread.
    virtual ScheduleStatus Status() const = 0;

    /// Requests cancellation.
    ///
    /// One-shot tasks: if the task is Scheduled it will not execute.
    /// Repeating tasks: no further invocations will be scheduled after
    ///   the current one (if any) completes.
    ///
    /// If the task is already Running when Cancel() is called, the
    /// current execution runs to completion; its status transitions to
    /// Cancelled once it returns, and no further scheduling occurs.
    ///
    /// Has no effect if the task has already Completed or been Cancelled.
    /// Safe to call from any thread, including from within the task itself.
    virtual void Cancel() = 0;

protected:
    Cancellable() = default;

    // Non-copyable
    Cancellable(const Cancellable&)            = delete;
    Cancellable& operator=(const Cancellable&) = delete;
};
