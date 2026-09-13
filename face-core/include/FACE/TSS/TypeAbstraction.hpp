// FACE/TSS/TypeAbstraction.hpp
// C++ mapping of FACE/TSS/TypeAbstraction.idl
// FACE Technical Standard Edition 3.2
//
// Type Abstraction (TA) interface used by the TS-TA adapter.
// Carries untyped MESSAGE_TYPE instead of a concrete application data type.

#ifndef FACE_TSS_TYPEABSTRACTION_HPP
#define FACE_TSS_TYPEABSTRACTION_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {
namespace TypeAbstraction {

//----------------------------------------------------------------------------
// Read_Callback
//----------------------------------------------------------------------------

/// Callback interface for periodic data reception at the TA level.
///
/// Register via TypeAbstractionTS::Register_Callback() to avoid polling.
class Read_Callback {
public:
    virtual ~Read_Callback() {}

    /// Called by the TA when a message becomes available.
    ///
    /// @param connection_id    Connection that received the message.
    /// @param transaction_id   Ties request/reply messages together.
    /// @param message          Raw untyped message.
    /// @param header           Instance UID, source UID, and timestamp.
    /// @param qos_parameters   QoS annotations for this delivery.
    /// @param return_code      Output: status of the callback.
    virtual void Callback_Handler(
        CONNECTION_ID_TYPE    connection_id,
        TRANSACTION_ID_TYPE   transaction_id,
        const MESSAGE_TYPE&   message,
        const HEADER_TYPE&    header,
        const QoS_EVENT_TYPE& qos_parameters,
        RETURN_CODE_TYPE&     return_code) = 0;
};

//----------------------------------------------------------------------------
// TypeAbstractionTS
//----------------------------------------------------------------------------

/// Type Abstraction Transport Services interface.
///
/// Provides send/receive operations over raw MESSAGE_TYPE buffers,
/// plus blocking and async variants for client/server patterns.
class TypeAbstractionTS {
public:
    virtual ~TypeAbstractionTS() {}

    /// Receive a message from the TA.
    ///
    /// If message_guid == CALLEE_PROVIDES_GUID the TA assigns the GUID and
    /// returns the value in message; otherwise the caller's GUID is used.
    ///
    /// @param connection_id    Connection to receive from.
    /// @param timeout          Maximum wait time; INF_TIME_VALUE blocks.
    /// @param transaction_id   Output: transaction identifier.
    /// @param size_limit       Maximum message size the caller will accept.
    /// @param message          Inout: populated with data on success;
    ///                         GUID updated if CALLEE_PROVIDES_GUID was set.
    /// @param header           Output: message header.
    /// @param qos_parameters   Output: QoS parameters for this delivery.
    /// @param return_code      Output: NO_ERROR, NOT_AVAILABLE, TIMED_OUT, etc.
    virtual void Receive_Message(
        CONNECTION_ID_TYPE    connection_id,
        TIMEOUT_TYPE          timeout,
        TRANSACTION_ID_TYPE&  transaction_id,
        MESSAGE_SIZE_TYPE     size_limit,
        MESSAGE_TYPE&         message,
        HEADER_TYPE&          header,
        QoS_EVENT_TYPE&       qos_parameters,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Send a message to the destination.
    ///
    /// @param connection_id    Connection to send on.
    /// @param timeout          Maximum wait time; INF_TIME_VALUE blocks.
    /// @param transaction_id   Inout: provided or callee-assigned TID.
    /// @param message          The untyped message to send.
    /// @param size_sent        Output: number of bytes actually sent.
    /// @param return_code      Output: NO_ERROR on success.
    virtual void Send_Message(
        CONNECTION_ID_TYPE    connection_id,
        TIMEOUT_TYPE          timeout,
        TRANSACTION_ID_TYPE&  transaction_id,
        const MESSAGE_TYPE&   message,
        MESSAGE_SIZE_TYPE&    size_sent,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Send a message and block until the response is received.
    ///
    /// @param connection_id    Connection to use.
    /// @param timeout          Maximum wait time; INF_TIME_VALUE blocks.
    /// @param size_limit       Maximum size the caller will accept for return_data.
    /// @param message          The request message to send.
    /// @param size_sent        Output: bytes sent.
    /// @param header           Output: header of the response.
    /// @param qos_parameters   Output: QoS parameters of the response.
    /// @param return_data      Inout: populated with response on success.
    /// @param return_code      Output: NO_ERROR on success.
    virtual void Send_Message_Blocking(
        CONNECTION_ID_TYPE    connection_id,
        TIMEOUT_TYPE          timeout,
        MESSAGE_SIZE_TYPE     size_limit,
        const MESSAGE_TYPE&   message,
        MESSAGE_SIZE_TYPE&    size_sent,
        HEADER_TYPE&          header,
        QoS_EVENT_TYPE&       qos_parameters,
        MESSAGE_TYPE&         return_data,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Send a message asynchronously; response delivered via callback.
    ///
    /// @param connection_id    Connection to use.
    /// @param timeout          Maximum wait time for the send.
    /// @param transaction_id   Inout: provided or callee-assigned TID.
    /// @param message          The request message to send.
    /// @param callback         Inout: callback object for the response
    ///                         (semantically in; inout for C++ mapping).
    /// @param size_sent        Output: bytes sent.
    /// @param return_code      Output: NO_ERROR on success.
    virtual void Send_Message_Async(
        CONNECTION_ID_TYPE    connection_id,
        TIMEOUT_TYPE          timeout,
        TRANSACTION_ID_TYPE&  transaction_id,
        const MESSAGE_TYPE&   message,
        Read_Callback&        callback,
        MESSAGE_SIZE_TYPE&    size_sent,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Register a callback to receive data without polling (pub/sub).
    ///
    /// message_guid == CALLEE_PROVIDES_GUID indicates the TS-TA does not
    /// supply GUIDs but expects the TA to source them.
    ///
    /// @param connection_id      Connection to register on.
    /// @param data_callback      Inout: the callback object.
    /// @param max_message_size   Maximum message size this callback handles.
    /// @param message_guid       Expected GUID, or CALLEE_PROVIDES_GUID.
    /// @param return_code        Output: NO_ERROR on success.
    virtual void Register_Callback(
        CONNECTION_ID_TYPE    connection_id,
        Read_Callback&        data_callback,
        MESSAGE_SIZE_TYPE     max_message_size,
        MESSAGE_GUID_TYPE     message_guid,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Unregister the callback previously registered for this connection.
    ///
    /// @param connection_id  Connection whose callback is to be removed.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Unregister_Callback(
        CONNECTION_ID_TYPE connection_id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace TypeAbstraction
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_TYPEABSTRACTION_HPP
