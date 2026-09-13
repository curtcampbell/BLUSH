// FACE/TSS/TPM.hpp
// C++ mapping of FACE/TSS/TPM.idl
// FACE Technical Standard Edition 3.2
//
// Transport Protocol Module (TPM) interface.
// Provides the primary TS with access to the underlying transport hardware
// and protocol binding module.

#ifndef FACE_TSS_TPM_HPP
#define FACE_TSS_TPM_HPP

#include "Common.hpp"
#include <vector>

namespace FACE {
namespace TSS {
namespace TPM {

//----------------------------------------------------------------------------
// Types
//----------------------------------------------------------------------------

/// Unique identifier for a TPM channel.
typedef UID_TYPE CHANNEL_ID_TYPE;

/// Sequence of channel IDs.
typedef std::vector<CHANNEL_ID_TYPE> CHANNEL_ID_SEQ_TYPE;

/// Transport event types reported by the TPM.
enum EVENT_TYPE {
    INIT_COMPLETE,      ///< initialisation has completed
    XPORT_DEGRADED,     ///< transport performance is being degraded (e.g. oversubscribed)
    CBIT_FAIL,          ///< continuous built-in-test failure
    IBIT_FAIL,          ///< initiated built-in-test failure
    CHANNEL_FAIL,       ///< a failure has occurred in a specific channel
    LOST_LINK,          ///< the transport link detects no wire activity
    TRANSMIT_COMPLETE   ///< the last transmission has completed
};

//----------------------------------------------------------------------------
// TPM_Callback
//----------------------------------------------------------------------------

/// Callback interface for data and event notifications from the TPM.
///
/// Two callback kinds: DATA (incoming datagrams) and EVENT (status changes).
class TPM_Callback {
public:
    virtual ~TPM_Callback() {}

    /// Numeric event code (implementation-defined supplementary detail).
    typedef int32_t EVENT_CODE_TYPE;

    /// Human-readable diagnostic message.
    typedef STRING_TYPE DIAGNOSTIC_MSG_TYPE;

    /// Identifies what kind of notification this callback handles.
    enum CALLBACK_KIND_TYPE {
        DATA,   ///< callback for a message incoming from the transport
        EVENT,  ///< callback for a change in event status
        BOTH    ///< callback for both a message and a change in event status
    };

    /// Returns the kind of callback this object handles (read-only attribute).
    virtual CALLBACK_KIND_TYPE callback_kind() const = 0;

    /// Called when a datagram arrives from the transport.
    ///
    /// @param channel_id       Channel that received the message.
    /// @param transaction_id   Transaction identifier.
    /// @param message          The raw untyped message.
    /// @param tss_header       TSS header for the message.
    /// @param qos_parameters   QoS annotations.
    /// @param return_code      Output: status of the callback.
    virtual void Data_Callback_Handler(
        CHANNEL_ID_TYPE       channel_id,
        TRANSACTION_ID_TYPE   transaction_id,
        const MESSAGE_TYPE&   message,
        const HEADER_TYPE&    tss_header,
        const QoS_EVENT_TYPE& qos_parameters,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Called when a transport event occurs (e.g. LOST_LINK, CBIT_FAIL).
    ///
    /// @param channel_id       Channel associated with the event.
    /// @param transaction_id   Transaction identifier.
    /// @param event            The event type.
    /// @param event_code       Implementation-defined detail code.
    /// @param diagnostic_msg   Human-readable diagnostic message.
    /// @param return_code      Output: status of the callback.
    virtual void Event_Callback_Handler(
        CHANNEL_ID_TYPE          channel_id,
        TRANSACTION_ID_TYPE      transaction_id,
        EVENT_TYPE               event,
        EVENT_CODE_TYPE          event_code,
        const DIAGNOSTIC_MSG_TYPE& diagnostic_msg,
        RETURN_CODE_TYPE&        return_code) = 0;
};

//----------------------------------------------------------------------------
// TPMTS
//----------------------------------------------------------------------------

/// Transport Protocol Module Transport Services interface.
///
/// Provides the primary TS with channel management, datagram I/O,
/// state control, and callback registration for the underlying transport.
class TPMTS {
public:
    virtual ~TPMTS() {}

    //----------------------------------------------------------
    // Nested types
    //----------------------------------------------------------

    /// TPM operational state.
    enum TPM_STATE_TYPE {
        NORMAL,    ///< normal operations
        TEST,      ///< performing a built-in test
        RESUME,    ///< resuming from PAUSE
        PAUSE,     ///< temporarily suspending communications
        SHUTDOWN,  ///< orderly shutdown
        SECURE     ///< secure/zeroise state
    };

    /// Test level for TPM_STATE_TYPE::TEST.
    enum LEVEL_OF_TEST_TYPE {
        CBIT,  ///< continuous built-in test
        IBIT,  ///< initiated built-in test
        PBIT   ///< power-on built-in test
    };

    /// Data accompanying a state-change request.
    ///
    /// This is a discriminated union in IDL; in C++ we represent it as a
    /// struct with an active-member tag plus storage for each variant.
    struct STATE_CHANGE_DATA_TYPE {
        TPM_STATE_TYPE state; ///< discriminator – which member is active

        union {
            /// Active when state == SECURE: channels whose data must be cleared.
            /// (NOTE: the channels_to_clear sequence is stored externally;
            ///  set state to SECURE and populate the channels_to_clear field.)
            CHANNEL_ID_SEQ_TYPE* channels_to_clear_ptr; ///< pointer; caller owns storage

            /// Active when state == TEST.
            LEVEL_OF_TEST_TYPE test_level;
        } data;

        STATE_CHANGE_DATA_TYPE()
            : state(NORMAL)
        {
            data.channels_to_clear_ptr = nullptr;
        }
    };

    //----------------------------------------------------------
    // Operations
    //----------------------------------------------------------

    /// Initialise the transport hardware and protocol.
    ///
    /// Call after all protocol binding module functions are registered.
    ///
    /// @param configuration  Resource locator for TPM configuration.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Initialize(
        const CONFIGURATION_RESOURCE& configuration,
        RETURN_CODE_TYPE&             return_code) = 0;

    /// Establish an endpoint connection with another TS domain.
    ///
    /// @param endpoint_name      Named configuration for the channel endpoint.
    /// @param transport_config   Transport-layer configuration buffer.
    /// @param security_config    Security-layer configuration buffer.
    /// @param channel_id         Output: identifier for the new channel.
    /// @param return_code        Output: NO_ERROR on success.
    virtual void Open_Channel(
        const CONNECTION_NAME_TYPE& endpoint_name,
        const DATA_BUFFER_TYPE&     transport_config,
        const DATA_BUFFER_TYPE&     security_config,
        CHANNEL_ID_TYPE&            channel_id,
        RETURN_CODE_TYPE&           return_code) = 0;

    /// Close an open channel and release its resources.
    ///
    /// @param channel_id  Channel to close.
    /// @param return_code Output: NO_ERROR on success.
    virtual void Close_Channel(
        CHANNEL_ID_TYPE   channel_id,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Request a TPM state transition.
    ///
    /// @param new_state    The state to transition to.
    /// @param data         Auxiliary data associated with the transition.
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Request_TPM_State_Change(
        TPM_STATE_TYPE               new_state,
        const STATE_CHANGE_DATA_TYPE& data,
        RETURN_CODE_TYPE&            return_code) = 0;

    /// Query which channels have data available (non-blocking polling).
    ///
    /// A null timeout (0) blocks until any data is received.
    ///
    /// @param channel_ids     Channels to query.
    /// @param timeout         Maximum wait; 0 blocks.
    /// @param available_ids   Output: channels with data available.
    /// @param return_code     Output: NO_ERROR on success.
    virtual void Is_Data_Available(
        const CHANNEL_ID_SEQ_TYPE& channel_ids,
        TIMEOUT_TYPE               timeout,
        CHANNEL_ID_SEQ_TYPE&       available_ids,
        RETURN_CODE_TYPE&          return_code) = 0;

    /// Query the overall health and availability of the transport.
    ///
    /// @param status       Output: current event/status code.
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Get_TPM_Status(
        EVENT_TYPE&       status,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Read an incoming datagram from the transport.
    ///
    /// With non-zero timeout, blocks until data arrives or timeout expires.
    /// With timeout == 0, returns immediately.
    ///
    /// @param channel_id       Channel to read from.
    /// @param timeout          Maximum wait time; 0 returns immediately.
    /// @param transaction_id   Output: transaction identifier.
    /// @param message          Inout: caller-supplied buffer; filled on success.
    /// @param TSS_header       Output: TSS header of the received datagram.
    /// @param qos_parameters   Output: QoS parameters.
    /// @param return_code      Output: NO_ERROR on success.
    virtual void Read_From_Transport(
        CHANNEL_ID_TYPE       channel_id,
        TIMEOUT_TYPE          timeout,
        TRANSACTION_ID_TYPE&  transaction_id,
        MESSAGE_TYPE&         message,
        HEADER_TYPE&          TSS_header,
        QoS_EVENT_TYPE&       qos_parameters,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Write a datagram to the transport for protocol processing.
    ///
    /// @param channel_id       Channel to transmit on.
    /// @param max_delay        Maximum pipeline delay; 0 sends immediately.
    /// @param message          The message to send.
    /// @param TSS_header       TSS header to include.
    /// @param transaction_id   Transaction identifier.
    /// @param return_code      Output: NO_ERROR on success.
    virtual void Write_To_Transport(
        CHANNEL_ID_TYPE       channel_id,
        TIMEOUT_TYPE          max_delay,
        const MESSAGE_TYPE&   message,
        const HEADER_TYPE&    TSS_header,
        TRANSACTION_ID_TYPE   transaction_id,
        RETURN_CODE_TYPE&     return_code) = 0;

    /// Register a callback for data or events on a channel.
    ///
    /// @param channel_id  Channel to register on.
    /// @param callback    Inout: callback object (semantically in; inout for C++ mapping).
    /// @param return_code Output: NO_ERROR on success.
    virtual void Register_TPM_Callback(
        CHANNEL_ID_TYPE   channel_id,
        TPM_Callback&     callback,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Unregister a callback from a channel.
    ///
    /// @param channel_id      Channel to unregister from.
    /// @param callback_kind   Kind of callback to remove (DATA, EVENT, or BOTH).
    /// @param return_code     Output: NO_ERROR on success.
    virtual void Unregister_TPM_Callback(
        CHANNEL_ID_TYPE                 channel_id,
        TPM_Callback::CALLBACK_KIND_TYPE callback_kind,
        RETURN_CODE_TYPE&               return_code) = 0;
};

} // namespace TPM
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_TPM_HPP
