#ifndef PUBLISHERCONNECTION_H
#define PUBLISHERCONNECTION_H

#include "FaceTypeTraits.h"
#include "FACE/Common.hpp"
#include "FACE/TSS/Common.hpp"

/// PublisherConnection<DataType>
///
/// Wraps a FACE TypedTS connection for publishing (sending) messages.
///
/// Template parameters:
///   DataType     — the FACE data model type to publish
///   TypedTS      — the TypedTS interface (resolved from Traits<DataType> by default)
///   ReadCallback — the read-callback type (resolved from Traits<DataType> by default;
///                  carried along for completeness, not used by publisher)
///
/// Usage:
///   PublisherConnection<NavModel::FlightPlan> conn(ts, connectionId);
///   auto rc = conn.send(flightPlan);
template<
    typename DataType,
    typename TypedTS      = typename Traits<DataType>::TypedTS,
    typename ReadCallback = typename Traits<DataType>::Read_Callback
>
class PublisherConnection {
public:
    /// Construct with the live TypedTS pointer and the connection ID to publish on.
    PublisherConnection(TypedTS* ts, FACE::TSS::CONNECTION_ID_TYPE connectionId)
        : m_ts(ts), m_connectionId(connectionId) {}

    // Non-copyable
    PublisherConnection(const PublisherConnection&) = delete;
    PublisherConnection& operator=(const PublisherConnection&) = delete;

    // Movable
    PublisherConnection(PublisherConnection&&) = default;
    PublisherConnection& operator=(PublisherConnection&&) = default;

    /// Send a message on the connection.
    ///
    /// @param message   The data to publish.
    /// @param timeout   Maximum wait time (default: INF_TIME_VALUE — block until sent).
    /// @returns         FACE return code from Send_Message.
    FACE::RETURN_CODE_TYPE send(
        const DataType&    message,
        FACE::TIMEOUT_TYPE timeout = FACE::INF_TIME_VALUE)
    {
        FACE::TSS::TRANSACTION_ID_TYPE transactionId = FACE::TSS::TID_NOT_APPLICABLE;
        FACE::RETURN_CODE_TYPE rc = FACE::RETURN_CODE_TYPE::NOT_AVAILABLE;
        m_ts->Send_Message(m_connectionId, timeout, transactionId, message, rc);
        return rc;
    }

private:
    TypedTS*                      m_ts;
    FACE::TSS::CONNECTION_ID_TYPE  m_connectionId;
};

#endif // PUBLISHERCONNECTION_H
