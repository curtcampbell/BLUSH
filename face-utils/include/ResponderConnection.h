#ifndef RESPONDERCONNECTION_H
#define RESPONDERCONNECTION_H

#include "FaceTypeTraits.h"
#include "EventDispatcher.h"
#include "EventRegistration.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Common.hpp"
#include <functional>
#include <memory>

/// ResponderConnection<RequestType, ResponseType, IResponseSenderBase>
///
/// Wraps the server/responder side of a FACE CLIENT_SERVER connection for
/// receiving requests and sending responses.  Inherits from
/// RequestReadCallback to serve as the registered FACE callback for the
/// request side.
///
/// FACE Technical Standard 3.2 Appendix E.3.2/E.3.3: unlike the client side
/// (RequesterConnection, which uses the Extended TypedTS's
/// Send_Message_Blocking/Send_Message_Async), the server side does NOT use
/// the Extended interface at all -- "Servers, publishers and subscribers do
/// not use Send_Message_Async(TS)". Instead the server uses two ordinary
/// Standard TypedTS connections:
///   - one instantiated with the REQUEST type, to receive requests exactly
///     like a pub/sub subscriber (Register_Callback/Callback_Handler), which
///     hands back a transaction_id ("Servers ... are returned a valid
///     transaction_id from the Receive_Message(TS) call");
///   - one instantiated with the RESPONSE type, to send the reply exactly
///     like a pub/sub publisher (Send_Message), passing that SAME
///     transaction_id back ("When sending a server's response, the TSS
///     implementation uses the transaction_id provided by the caller").
///
/// The two TypedTS connections still share a single CONNECTION_ID_TYPE (FACE
/// Technical Standard §E.3, RIG Vol 2 §6.4.2.2): one Create_Connection call
/// yields one id used for both Register_Callback (inbound requests) and
/// Send_Message (outbound responses).
///
/// For each incoming request, a stack-allocated ResponseSenderImpl is
/// constructed and passed alongside the request to all registered handlers.
/// Handlers call sender.sendResponse() to dispatch the reply.
///
/// Each registration carries an optional predicate:
///   std::function<bool(const RequestType&)>
/// Before a request is forwarded to a handler, its predicate is evaluated.
/// An empty (default-constructed) predicate is treated as always-true.
///
/// Lazy open/close: Register_Callback is called the first time a handler is
/// registered; Unregister_Callback is called when the last handler is removed.
///
/// Non-copyable and non-movable — this pointer is registered with the FACE TS.
///
/// Template parameters:
///   RequestType         — the FACE request data model type
///   ResponseType        — the FACE response data model type
///   IResponseSenderBase — generated I{TypeName}ResponseSender interface;
///                         must declare sendResponse(const ResponseType&)
///   RequestTypedTS      — Standard TypedTS for the request type (receive)
///                         (default: from Traits<RequestType>)
///   RequestReadCallback — request-side callback base class
///                         (default: from Traits<RequestType>)
///   ResponseTypedTS     — Standard TypedTS for the response type (send)
///                         (default: from Traits<ResponseType>)
///
/// Usage:
///   ResponderConnection<NavModel::Request, NavModel::Response, IMyResponseSender>
///       conn(requestTs, responseTs, connectionId);
///
///   auto reg = conn.registerHandler(
///       [](const NavModel::Request& req, IMyResponseSender& sender) {
///           sender.sendResponse(buildResponse(req));
///       });
template<
    typename RequestType,
    typename ResponseType,
    typename IResponseSenderBase,
    typename RequestTypedTS      = typename Traits<RequestType>::TypedTS,
    typename RequestReadCallback = typename Traits<RequestType>::Read_Callback,
    typename ResponseTypedTS     = typename Traits<ResponseType>::TypedTS
>
class ResponderConnection : public RequestReadCallback {
public:
    using HandlerFn   = std::function<void(const RequestType&, IResponseSenderBase&)>;
    using PredicateFn = std::function<bool(const RequestType&)>;

    /// @param requestTs     Standard TypedTS for the request type (receive side).
    /// @param responseTs    Standard TypedTS for the response type (send side).
    /// @param connectionId  The single connection ID returned by Base::Create_Connection.
    ///                      Used for both Register_Callback (on requestTs) and
    ///                      Send_Message (on responseTs).
    ResponderConnection(
        RequestTypedTS*                requestTs,
        ResponseTypedTS*               responseTs,
        FACE::TSS::CONNECTION_ID_TYPE  connectionId)
        : m_requestTs(requestTs)
        , m_responseTs(responseTs)
        , m_connectionId(connectionId)
        , m_handlerCount(0)
    {}

    ~ResponderConnection() {
        if (m_handlerCount > 0) {
            FACE::RETURN_CODE_TYPE rc;
            m_requestTs->Unregister_Callback(m_connectionId, rc);
        }
    }

    // Non-copyable, non-movable
    ResponderConnection(const ResponderConnection&) = delete;
    ResponderConnection& operator=(const ResponderConnection&) = delete;
    ResponderConnection(ResponderConnection&&) = delete;
    ResponderConnection& operator=(ResponderConnection&&) = delete;

    /// Register a request handler.
    ///
    /// @param handler    Callable invoked with (request, responseSender) when
    ///                   the predicate passes.
    /// @param predicate  Optional filter; called with each request before the
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
                        p = std::move(predicate)](
                            const RequestType& req, IResponseSenderBase& sender)
        {
            if (!p || p(req)) { h(req, sender); }
        };

        auto reg = m_dispatcher.Register(std::move(wrapped));

        if (m_handlerCount == 0) {
            // Register_Callback's callback parameter is "inout Read_Callback"
            // where Read_Callback is a LOCAL interface (declared alongside
            // TypedTS in the same Typed<DATATYPE_TYPE> template body) --
            // FACE TS 3.2's C++ mapping for that shape is
            // RequestReadCallback**, not a reference (confirmed against real
            // generated TypedTS.hpp; same fix as SubscriberConnection).
            FACE::RETURN_CODE_TYPE rc;
            RequestReadCallback* selfPtr = this;
            m_requestTs->Register_Callback(m_connectionId, &selfPtr, rc);
        }
        ++m_handlerCount;

        struct HandlerRegistration : public EventRegistration {
            std::shared_ptr<EventRegistration> m_inner;
            ResponderConnection*               m_owner;
            bool                               m_cancelled;

            HandlerRegistration(
                std::shared_ptr<EventRegistration> inner,
                ResponderConnection*               owner)
                : m_inner(std::move(inner)), m_owner(owner), m_cancelled(false) {}

            void Cancel() override {
                if (!m_cancelled) {
                    m_cancelled = true;
                    m_inner->Cancel();
                    --(m_owner->m_handlerCount);
                    if (m_owner->m_handlerCount == 0) {
                        FACE::RETURN_CODE_TYPE rc;
                        m_owner->m_requestTs->Unregister_Callback(
                            m_owner->m_connectionId, rc);
                    }
                }
            }
        };

        return std::make_shared<HandlerRegistration>(std::move(reg), this);
    }

    // --- FACE RequestReadCallback implementation ---
    void Callback_Handler(
        FACE::TSS::CONNECTION_ID_TYPE    connection_id,
        FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
        const RequestType&               message,
        const FACE::TSS::HEADER_TYPE&    header,
        const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
        FACE::RETURN_CODE_TYPE&          return_code) override
    {
        (void)connection_id;
        (void)header;
        (void)qos_parameters;

        // The transaction_id from the inbound request is used to correlate
        // the outbound response on the same connection ID.
        ResponseSenderImpl sender(m_responseTs, m_connectionId, transaction_id);
        m_dispatcher(message, sender);
        return_code = FACE::RETURN_CODE_TYPE::NO_ERROR;
    }

private:
    /// Stack-allocated response sender created per incoming request.
    /// Wraps the response TypedTS and connection ID; uses the inbound
    /// transaction_id to correlate request and response.
    struct ResponseSenderImpl : public IResponseSenderBase {
        ResponseSenderImpl(
            ResponseTypedTS*               ts,
            FACE::TSS::CONNECTION_ID_TYPE  connectionId,
            FACE::TSS::TRANSACTION_ID_TYPE transactionId)
            : m_ts(ts)
            , m_connectionId(connectionId)
            , m_transactionId(transactionId)
        {}

        void sendResponse(const ResponseType& response) override {
            FACE::RETURN_CODE_TYPE rc;
            m_ts->Send_Message(
                m_connectionId,
                FACE::INF_TIME_VALUE,
                m_transactionId,
                response,
                rc);
        }

        ResponseTypedTS*               m_ts;
        FACE::TSS::CONNECTION_ID_TYPE  m_connectionId;
        FACE::TSS::TRANSACTION_ID_TYPE m_transactionId;
    };

    RequestTypedTS*                 m_requestTs;
    ResponseTypedTS*                m_responseTs;
    FACE::TSS::CONNECTION_ID_TYPE   m_connectionId;
    EventDispatcher<const RequestType&, IResponseSenderBase&> m_dispatcher;
    int                             m_handlerCount;
};

#endif // RESPONDERCONNECTION_H
