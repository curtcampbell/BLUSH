#pragma once

// EventRegistration.h
//
// Handle returned by EventDispatcher::Register() that lets a caller remove
// a previously registered callback.
//
// Callers hold an instance through std::shared_ptr<EventRegistration>.
// The concrete implementation is a private nested class inside
// EventDispatcher and is never exposed to callers directly.
//
// Lifetime safety
// ---------------
// Cancel() is safe to call after the originating EventDispatcher has been
// destroyed — the implementation holds a std::weak_ptr back to the
// dispatcher's shared state and treats a failed lock as a no-op.
// Cancel() is also idempotent: calls after the first are no-ops.
//
// Usage
// -----
//   std::shared_ptr<EventRegistration> reg =
//       dispatcher.Register([](int x) { ... });
//
//   // ... later, when the callback is no longer needed ...
//   reg->Cancel();

class EventRegistration
{
public:
    virtual ~EventRegistration() = default;

    /// Remove the associated callback from its EventDispatcher.
    ///
    /// Idempotent — safe to call more than once; calls after the first
    /// are no-ops.  Safe to call after the EventDispatcher has been
    /// destroyed.
    virtual void Cancel() = 0;

protected:
    EventRegistration() = default;

    // Non-copyable
    EventRegistration(const EventRegistration&)            = delete;
    EventRegistration& operator=(const EventRegistration&) = delete;
};
