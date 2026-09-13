// FACE/TSS/Extended.hpp
// C++ representation of the IDL template module:
//   module Typed<typename DATATYPE_TYPE, typename RESPONSE_DATATYPE> { ... }
// FACE Technical Standard Edition 3.2
//
// +--------------------------------------------------------------------------+
// |  UNINSTANTIATED IDL TEMPLATE MODULE                                      |
// |                                                                          |
// |  The FACE 3.2 IDL defines the Extended Typed template (two type          |
// |  parameters: request type and response type) but provides NO             |
// |  instantiation in the standard.  Instantiation is left to the system    |
// |  integrator or UoC developer.                                            |
// |                                                                          |
// |  CORRECT IDL-FIRST EXPANSION PROCESS (for reference):                   |
// |                                                                          |
// |  Step 1 - IDL instantiation (in your own .idl file):                    |
// |    module Typed<MyApp::Request, MyApp::Response>                         |
// |        Request_Response_TypedTS;                                         |
// |                                                                          |
// |  Step 2 - Expand the template body substituting MyApp::Request for      |
// |    DATATYPE_TYPE and MyApp::Response for RESPONSE_DATATYPE.             |
// |                                                                          |
// |  Step 3 - Apply C++ language binding to the expanded concrete IDL:      |
// |    namespace FACE { namespace TSS { namespace Request_Response_TypedTS { |
// |      class Read_Callback { ... Callback_Handler(..., const Response&,   |
// |        ...); };                                                          |
// |      class TypedTS { ... Send_Message_Blocking(..., const Request&,     |
// |          ..., Response&, ...);                                           |
// |                          Send_Message_Async(..., const Request&,        |
// |          ..., Read_Callback&, ...); };                                   |
// |    }}}                                                                   |
// |                                                                          |
// |  The C++ template below is a convenience representation ONLY.           |
// +--------------------------------------------------------------------------+

#ifndef FACE_TSS_EXTENDED_HPP
#define FACE_TSS_EXTENDED_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {

/// C++ template representation of IDL:
///   module Typed<typename DATATYPE_TYPE, typename RESPONSE_DATATYPE> { ... }
///
/// USAGE: This template is provided as a reference for the interface shape.
/// For a conformant FACE implementation, instantiate and expand the IDL per
/// the 3-step process described in the file header comment.
template<typename DATATYPE_TYPE, typename RESPONSE_DATATYPE>
struct Extended {

    /// Callback interface for asynchronous response reception.
    class Read_Callback {
    public:
        virtual ~Read_Callback() {}

        /// Called by the TS when a response arrives.
        ///
        /// @param connection_id    Connection the response arrived on.
        /// @param transaction_id   Ties this response to its originating request.
        /// @param message          The response data.
        /// @param header           Instance UID, source UID, and timestamp.
        /// @param qos_parameters   QoS annotations for this delivery.
        /// @param return_code      Output: status of the callback.
        virtual void Callback_Handler(
            CONNECTION_ID_TYPE        connection_id,
            TRANSACTION_ID_TYPE       transaction_id,
            const RESPONSE_DATATYPE&  message,
            const HEADER_TYPE&        header,
            const QoS_EVENT_TYPE&     qos_parameters,
            RETURN_CODE_TYPE&         return_code) = 0;
    };

    /// Extended typed TS interface -- synchronous blocking send and
    /// asynchronous send with callback.
    class TypedTS {
    public:
        virtual ~TypedTS() {}

        /// Send a message and block until the response is received.
        ///
        /// @param connection_id   Connection to use.
        /// @param timeout         Maximum wait time; INF_TIME_VALUE blocks.
        /// @param message         The request data to send.
        /// @param header          Output: header of the response.
        /// @param qos_parameters  Output: QoS parameters of the response.
        /// @param return_data     Inout: populated with response data on success.
        /// @param return_code     Output: NO_ERROR on success.
        virtual void Send_Message_Blocking(
            CONNECTION_ID_TYPE    connection_id,
            TIMEOUT_TYPE          timeout,
            const DATATYPE_TYPE&  message,
            HEADER_TYPE&          header,
            QoS_EVENT_TYPE&       qos_parameters,
            RESPONSE_DATATYPE&    return_data,
            RETURN_CODE_TYPE&     return_code) = 0;

        /// Send a message asynchronously; response delivered via callback.
        ///
        /// @param connection_id   Connection to use.
        /// @param timeout         Maximum wait time for the send.
        /// @param transaction_id  Inout: provided or callee-assigned TID.
        /// @param message         The request data to send.
        /// @param callback        Inout: callback for the response.
        /// @param return_code     Output: NO_ERROR on success.
        virtual void Send_Message_Async(
            CONNECTION_ID_TYPE    connection_id,
            TIMEOUT_TYPE          timeout,
            TRANSACTION_ID_TYPE&  transaction_id,
            const DATATYPE_TYPE&  message,
            Read_Callback&        callback,
            RETURN_CODE_TYPE&     return_code) = 0;
    };

}; // struct Extended<DATATYPE_TYPE, RESPONSE_DATATYPE>

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_EXTENDED_HPP
