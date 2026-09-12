#ifndef SUBSCRIBERCONNECTION_H
#define SUBSCRIBERCONNECTION_H

#include "FaceTypeTraits.h"
#include "EventDispatcher.h"
#include "EventRegistration.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Common.hpp"
#include <functional>
#include <memory>

/// SubscriberConnection<DataType>
///
/// Wraps a FACE TypedTS connection for receiving messages asynchronously.
/// Inherits from ReadCallback to serve as the registered FACE callback.
///
/// Lazy open/close: Register_Callback is called the first time a handler is
/// registered; Unregister_Callback is called when the last handler is removed.
///
/// Each registration carries an optional predicate:
///   std::function<bool(const DataType&)>
/// Before a message is forwarded to a handler, its predicate is evaluated.
/// An empty (default-constructed) predicate is treated as always-true.
///
/// Non-copyable and non-movable — this pointer is registered with the FACE TS.
///
/// Usage:
///   SubscriberConnection<NavModel::FlightPlan> conn(ts, connectionId);
///
///   // No predicate — receives all messages
///   auto reg1 = conn.registerHandler([](const NavModel::FlightPlan& fp) { ... });
///
///   // With predicate — receives only matching messages
///   auto reg2 = conn.registerHandler(
///       [](const NavModel::FlightPlan& fp) { ... },
///       [](const NavModel::FlightPlan& fp) { return fp.priority > 5; });
template<
    typename DataType,
    typename TypedTS      = typename Traits<DataType>::TypedTS,
    typename ReadCallback = typename Traits<DataType>::Read_Callback
>
class SubscriberConnection : public ReadCallback {
public:
    using HandlerFn   = std::function<void(const DataType&)>;
    using PredicateFn = std::function<bool(const DataType&)>;

    SubscriberConnection(TypedTS* ts, FACE::TSS::CONNECTION_ID_TYPE connectionId)
        : m_ts(ts), m_connectionId(connectionId), m_handlerCount(0) {}

    ~SubscriberConnection() {
        if (m_handlerCount > 0) {
            FACE::RETURN_CODE_TYPE rc;
            m_ts->Unregister_Callback(m_connectionId, rc);
        }
    }

    // Non-copyable, non-movable
    SubscriberConnection(const SubscriberConnection&) = delete;
    SubscriberConnection& operator=(const SubscriberConnection&) = delete;
    SubscriberConnection(SubscriberConnection&&) = delete;
    SubscriberConnection& operator=(SubscriberConnection&&) = delete;

    /// Register a handler for incoming messages.
    ///
    /// @param handler    Callable invoked with the received DataType when the
    ///                   predicate passes.
    /// @param predicate  Optional filter; called with each message before the
    ///                   handler.  An empty (default) predicate always passes.
    /// @returns          Registration handle; Cancel() or let it expire to
    ///                   unsubscribe.
    std::shared_ptr<EventRegistration> registerHandler(
        HandlerFn   handler,
        PredicateFn predicate = {})
    {
        // Wrap handler and predicate together so the dispatcher only needs to
        // hold a single callable.  Empty predicate is treated as always-true.
        auto wrapped = [h = std::move(handler),
                        p = std::move(predicate)](const DataType& msg)
        {
            if (!p || p(msg)) { h(msg); }
        };

        auto reg = m_dispatcher.Register(std::move(wrapped));

        if (m_handlerCount == 0) {
            FACE::RETURN_CODE_TYPE rc;
            m_ts->Register_Callback(m_connectionId, *this, rc);
        }
        ++m_handlerCount;

        struct HandlerRegistration : public EventRegistration {
            std::shared_ptr<EventRegistration> m_inner;
            SubscriberConnection*              m_owner;
            bool                               m_cancelled;

            HandlerRegistration(
                std::shared_ptr<EventRegistration> inner,
                SubscriberConnection*              owner)
                : m_inner(std::move(inner)), m_owner(owner), m_cancelled(false) {}

            void Cancel() override {
                if (!m_cancelled) {
                    m_cancelled = true;
                    m_inner->Cancel();
                    --(m_owner->m_handlerCount);
                    if (m_owner->m_handlerCount == 0) {
                        FACE::RETURN_CODE_TYPE rc;
                        m_owner->m_ts->Unregister_Callback(m_owner->m_connectionId, rc);
                    }
                }
            }
        };

        return std::make_shared<HandlerRegistration>(std::move(reg), this);
    }

    // --- FACE ReadCallback implementation ---
    void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE    connection_id,
        FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
        const DataType&                  message,
        const FACE::TSS::HEADER_TYPE&    header,
        const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
        FACE::RETURN_CODE_TYPE&          return_code) override
    {
        (void)connection_id;
        (void)transaction_id;
        (void)header;
        (void)qos_parameters;
        m_dispatcher(message);
        return_code = FACE::RETURN_CODE_TYPE::NO_ERROR;
    }

private:
    TypedTS*                         m_ts;
    FACE::TSS::CONNECTION_ID_TYPE    m_connectionId;
    EventDispatcher<const DataType&> m_dispatcher;
    int                              m_handlerCount;
};

#endif // SUBSCRIBERCONNECTION_H
