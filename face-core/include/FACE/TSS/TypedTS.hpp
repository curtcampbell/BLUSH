// FACE/TSS/TypedTS.hpp
// C++ representation of the IDL template module:
//   module Typed<typename DATATYPE_TYPE> { ... }
// FACE Technical Standard Edition 3.2
//
// +--------------------------------------------------------------------------+
// |  UNINSTANTIATED IDL TEMPLATE MODULE                                      |
// |                                                                          |
// |  The FACE 3.2 IDL defines module Typed<typename DATATYPE_TYPE> but       |
// |  provides NO instantiation in the standard.  Instantiation is left to   |
// |  the system integrator or UoC developer.                                 |
// |                                                                          |
// |  CORRECT IDL-FIRST EXPANSION PROCESS (for reference):                   |
// |                                                                          |
// |  Step 1 - IDL instantiation (in your own .idl file):                    |
// |    module Typed<MyApp::FlightData> FlightData_TypedTS;                   |
// |                                                                          |
// |  Step 2 - Expand the template body by substituting MyApp::FlightData    |
// |    for every occurrence of DATATYPE_TYPE in the Typed<> module body.    |
// |                                                                          |
// |  Step 3 - Apply C++ language binding to the expanded concrete IDL:      |
// |    namespace FACE { namespace TSS { namespace FlightData_TypedTS {       |
// |      class Read_Callback { ... Callback_Handler(..., const FlightData&,  |
// |        ...); };                                                          |
// |      class TypedTS { ... Receive_Message(..., FlightData&, ...);         |
// |                          Send_Message(..., const FlightData&, ...); };   |
// |    }}}                                                                   |
// |                                                                          |
// |  The C++ template below is a convenience representation ONLY.  It does  |
// |  NOT model the correct IDL expansion (which produces a concrete          |
// |  namespace, not a C++ template class).  Use it only as a reference      |
// |  for the interface shape, and expand it per the 3-step process above    |
// |  when building your FACE-conformant implementation.                      |
// +--------------------------------------------------------------------------+

#ifndef FACE_TSS_TYPEDTS_HPP
#define FACE_TSS_TYPEDTS_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {

/// C++ template representation of IDL:
///   module Typed<typename DATATYPE_TYPE> { ... }
///
/// USAGE: This template is provided as a reference for the interface shape.
/// For a conformant FACE implementation, instantiate and expand the IDL per
/// the 3-step process described in the file header comment.
template<typename DATATYPE_TYPE>
struct Typed {

    /// Callback interface for asynchronous data reception.
    class Read_Callback {
    public:
        virtual ~Read_Callback() {}

        /// Called by the TS when a message arrives on the registered connection.
        ///
        /// @param connection_id    The connection that received the message.
        /// @param transaction_id   Ties request/reply messages together.
        /// @param message          The received application data.
        /// @param header           Instance UID, source UID, and timestamp.
        /// @param qos_parameters   QoS annotations for this delivery.
        /// @param return_code      Output: status of the callback.
        virtual void Callback_Handler(
            CONNECTION_ID_TYPE    connection_id,
            TRANSACTION_ID_TYPE   transaction_id,
            const DATATYPE_TYPE&  message,
            const HEADER_TYPE&    header,
            const QoS_EVENT_TYPE& qos_parameters,
            RETURN_CODE_TYPE&     return_code) = 0;
    };

    /// Typed Transport Services interface for one application data type.
    class TypedTS {
    public:
        virtual ~TypedTS() {}

        /// Receive a message from the transport (blocking or polling).
        ///
        /// @param connection_id    Connection to receive from.
        /// @param timeout          Maximum wait time; INF_TIME_VALUE blocks.
        /// @param transaction_id   Output: transaction identifier.
        /// @param message          Inout: populated with the received data on success.
        /// @param header           Output: message header.
        /// @param qos_parameters   Output: QoS parameters for this delivery.
        /// @param return_code      Output: NO_ERROR, NOT_AVAILABLE, TIMED_OUT, etc.
        virtual void Receive_Message(
            CONNECTION_ID_TYPE    connection_id,
            TIMEOUT_TYPE          timeout,
            TRANSACTION_ID_TYPE&  transaction_id,
            DATATYPE_TYPE&        message,
            HEADER_TYPE&          header,
            QoS_EVENT_TYPE&       qos_parameters,
            RETURN_CODE_TYPE&     return_code) = 0;

        /// Send a message to the destination.
        ///
        /// @param connection_id    Connection to send on.
        /// @param timeout          Maximum wait time; INF_TIME_VALUE blocks.
        /// @param transaction_id   Inout: provided or callee-assigned TID.
        /// @param message          The application data to send.
        /// @param return_code      Output: NO_ERROR on success.
        virtual void Send_Message(
            CONNECTION_ID_TYPE    connection_id,
            TIMEOUT_TYPE          timeout,
            TRANSACTION_ID_TYPE&  transaction_id,
            const DATATYPE_TYPE&  message,
            RETURN_CODE_TYPE&     return_code) = 0;

        /// Register a callback to receive data asynchronously.
        ///
        /// @param connection_id  Connection to monitor.
        /// @param callback       The callback object to register.
        /// @param return_code    Output: NO_ERROR on success.
        virtual void Register_Callback(
            CONNECTION_ID_TYPE connection_id,
            Read_Callback&     callback,
            RETURN_CODE_TYPE&  return_code) = 0;

        /// Unregister the previously registered callback for a connection.
        ///
        /// @param connection_id  Connection whose callback to remove.
        /// @param return_code    Output: NO_ERROR on success.
        virtual void Unregister_Callback(
            CONNECTION_ID_TYPE connection_id,
            RETURN_CODE_TYPE&  return_code) = 0;
    };

}; // struct Typed<DATATYPE_TYPE>

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_TYPEDTS_HPP
