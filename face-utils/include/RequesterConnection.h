#ifndef REQUESTERCONNECTION_H
#define REQUESTERCONNECTION_H

#include "FaceTypeTraits.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Common.hpp"
#include <functional>
#include <map>
#include <memory>

/// RequesterConnection<RequestType, ResponseType>
///
/// Wraps a FACE CLIENT_SERVER TypedTS connection for sending requests and
/// receiving responses asynchronously via Send_Message_Async.
///
/// Each send() call accepts the request and a one-shot response handler.
/// A per-call AsyncCallbackAdapter (inheriting ReadCallback) is registered
/// with the FACE TS and stored by transaction_id until the response arrives.
/// Completed adapters are removed lazily at the start of each subsequent send().
///
/// Non-copyable; movable.
///
/// Usage:
///   RequesterConnection<NavModel::Request, NavModel::Response> conn(ts, connectionId);
///   conn.send(request, [](const NavModel::Response& r) { ... });
template<
    typename RequestType,
    typename ResponseType,
    typename TypedTS      = typename Traits<RequestType>::TypedTS,
    typename ReadCallback = typename Traits<RequestType>::Read_Callback
>
class RequesterConnection {
public:
    using ResponseHandlerFn = std::function<void(const ResponseType&)>;

    RequesterConnection(TypedTS* ts, FACE::TSS::CONNECTION_ID_TYPE connectionId)
        : m_ts(ts), m_connectionId(connectionId) {}

    ~RequesterConnection() = default;

    // Non-copyable
    RequesterConnection(const RequesterConnection&) = delete;
    RequesterConnection& operator=(const RequesterConnection&) = delete;

    // Movable
    RequesterConnection(RequesterConnection&&) = default;
    RequesterConnection& operator=(RequesterConnection&&) = default;

    /// Send a request and register a one-shot response handler.
    ///
    /// Completed adapters from previous calls are removed before each send.
    ///
    /// @param request          The request message to send.
    /// @param responseHandler  Callback invoked once when the response arrives.
    /// @param timeout          Maximum wait time (default: INF_TIME_VALUE).
    /// @returns                FACE return code from Send_Message_Async.
    FACE::RETURN_CODE_TYPE send(
        const RequestType& request,
        ResponseHandlerFn  responseHandler,
        FACE::TIMEOUT_TYPE timeout = FACE::INF_TIME_VALUE)
    {
        cleanupCompleted();

        auto adapter = std::make_shared<AsyncCallbackAdapter>(std::move(responseHandler));
        ReadCallback* callbackPtr = adapter.get();

        FACE::TSS::TRANSACTION_ID_TYPE transactionId = FACE::TSS::CALLEE_PROVIDES_TID;
        FACE::RETURN_CODE_TYPE rc = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;

        m_ts->Send_Message_Async(
            m_connectionId, timeout, transactionId, request, &callbackPtr, rc);

        if (rc == FACE::RETURN_CODE_TYPE::NO_ERROR ||
            rc == FACE::RETURN_CODE_TYPE::IN_PROGRESS) {
            m_pendingCallbacks[transactionId] = std::move(adapter);
        }

        return rc;
    }

private:
    /// One-shot callback adapter: invokes the user handler then marks itself complete.
    /// The FACE TS calls Callback_Handler when the response arrives.
    struct AsyncCallbackAdapter : public ReadCallback {
        explicit AsyncCallbackAdapter(ResponseHandlerFn handler)
            : m_handler(std::move(handler)), m_complete(false) {}

        void Callback_Handler(
            FACE::TSS::CONNECTION_ID_TYPE    connection_id,
            FACE::TSS::TRANSACTION_ID_TYPE   transaction_id,
            const ResponseType&              message,
            const FACE::TSS::HEADER_TYPE&    header,
            const FACE::TSS::QoS_EVENT_TYPE& qos_parameters,
            FACE::RETURN_CODE_TYPE&          return_code) override
        {
            (void)connection_id;
            (void)transaction_id;
            (void)header;
            (void)qos_parameters;
            m_handler(message);
            m_complete = true;
            return_code = FACE::RETURN_CODE_TYPE::NO_ERROR;
        }

        ResponseHandlerFn m_handler;
        bool              m_complete;
    };

    void cleanupCompleted() {
        for (auto it = m_pendingCallbacks.begin(); it != m_pendingCallbacks.end(); ) {
            if (it->second->m_complete) {
                it = m_pendingCallbacks.erase(it);
            } else {
                ++it;
            }
        }
    }

    TypedTS*                      m_ts;
    FACE::TSS::CONNECTION_ID_TYPE  m_connectionId;
    std::map<FACE::TSS::TRANSACTION_ID_TYPE,
             std::shared_ptr<AsyncCallbackAdapter>> m_pendingCallbacks;
};

#endif // REQUESTERCONNECTION_H
