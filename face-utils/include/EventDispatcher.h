#pragma once

// EventDispatcher.h
//
// A multicast functor that dispatches a call to all registered callbacks,
// modelled after the multicast delegate in .NET.
//
// Template arguments specify the parameter types forwarded to each callback.
// All callbacks must have a void return type.
//
// Registration
// ------------
// Call Register() with any callable (lambda, free function, or bound member
// function via a capturing lambda or std::bind) to subscribe.  Register()
// returns a std::shared_ptr<EventRegistration>.  Calling Cancel() on that
// handle removes the callback; it is safe to call after the EventDispatcher
// itself has been destroyed.
//
// Dispatch
// --------
// operator() invokes all currently registered callbacks in registration order.
// A snapshot of the callback list is taken before dispatch begins, so
// cancellations or new registrations performed inside a callback do not affect
// the set of callbacks invoked during the current call.
//
// Thread safety
// -------------
// EventDispatcher is NOT thread-safe.  Register(), Cancel(), and operator()
// must not be called concurrently without external synchronisation.
//
// C++ standard note
// -----------------
// Requires C++14.
//
// Usage
// -----
//   EventDispatcher<int, const std::string&> onMessage;
//
//   auto reg = onMessage.Register([](int code, const std::string& text) {
//       std::cout << code << ": " << text << "\n";
//   });
//
//   onMessage(42, "hello");   // invokes all registered callbacks
//
//   reg->Cancel();            // unsubscribes; subsequent dispatches skip it

#include "EventRegistration.h"

#include <functional>
#include <map>
#include <memory>
#include <vector>

template<typename... Args>
class EventDispatcher
{
public:
    /// Construct an empty dispatcher with no registered callbacks.
    EventDispatcher()
        : m_state(std::make_shared<State>())
    {}

    // Non-copyable: copying would duplicate the callback list but any
    // existing EventRegistration handles would still cancel from the original
    // dispatcher, producing confusing ownership semantics.
    EventDispatcher(const EventDispatcher&)            = delete;
    EventDispatcher& operator=(const EventDispatcher&) = delete;

    // Movable: existing handles hold a std::weak_ptr to m_state, which
    // continues to reference the same State object after the move.
    EventDispatcher(EventDispatcher&&)            = default;
    EventDispatcher& operator=(EventDispatcher&&) = default;

    // -----------------------------------------------------------------------
    // Registration
    // -----------------------------------------------------------------------

    /// Subscribe a callback to be invoked on each call to operator().
    ///
    /// @param callback  Any callable with signature void(Args...).
    /// @return  A handle whose Cancel() method removes this callback.
    ///          Dropping the handle without calling Cancel() leaves the
    ///          callback registered.
    std::shared_ptr<EventRegistration> Register(std::function<void(Args...)> callback)
    {
        const int id = m_state->m_nextId++;
        m_state->m_callbacks.emplace(id, std::move(callback));
        return std::make_shared<RegistrationHandle>(m_state, id);
    }

    // -----------------------------------------------------------------------
    // Dispatch
    // -----------------------------------------------------------------------

    /// Invoke all registered callbacks in registration order.
    ///
    /// A snapshot of the callback list is made before dispatch begins, so
    /// cancellations or new registrations performed by a callback do not
    /// alter the set of callbacks invoked during this call.
    void operator()(Args... args) const
    {
        // Snapshot the live callback list before invoking so that any
        // mutation of m_callbacks during dispatch is safely ignored.
        std::vector<std::function<void(Args...)>> snapshot;
        snapshot.reserve(m_state->m_callbacks.size());
        for (const auto& kv : m_state->m_callbacks)
            snapshot.push_back(kv.second);

        for (auto& fn : snapshot)
            fn(args...);
    }

private:
    // -----------------------------------------------------------------------
    // Shared state — owned by the dispatcher, weakly referenced by handles
    // -----------------------------------------------------------------------

    struct State
    {
        std::map<int, std::function<void(Args...)>> m_callbacks;
        int                                         m_nextId = 0;
    };

    // -----------------------------------------------------------------------
    // Private EventRegistration implementation
    // -----------------------------------------------------------------------

    class RegistrationHandle : public EventRegistration
    {
    public:
        RegistrationHandle(std::weak_ptr<State> state, int id)
            : m_state(std::move(state))
            , m_id(id)
        {}

        /// Remove the callback from the dispatcher.
        ///
        /// No-op if Cancel() has already been called, or if the
        /// EventDispatcher has been destroyed.
        void Cancel() override
        {
            if (auto state = m_state.lock())
            {
                state->m_callbacks.erase(m_id);
                m_state.reset();   // ensure subsequent calls are no-ops
                                   // without attempting a second lock
            }
        }

    private:
        std::weak_ptr<State> m_state;
        int                  m_id;
    };

    // -----------------------------------------------------------------------
    // Data
    // -----------------------------------------------------------------------

    std::shared_ptr<State> m_state;
};
