// FACE/TSS/Common.hpp
// C++ mapping of FACE/TSS/Common.idl
// FACE Technical Standard Edition 3.2
//
// Common types shared across the Transport Services Segment (TSS) interfaces.

#ifndef FACE_TSS_COMMON_HPP
#define FACE_TSS_COMMON_HPP

#include "../Common.hpp"
#include <vector>  // for unbounded sequence (QoS_EVENT_TYPE)

namespace FACE {
namespace TSS {

//----------------------------------------------------------------------------
// Connection / Message identity types
//----------------------------------------------------------------------------

/// Name of the TSS connection used in Create_Connection().
typedef STRING_TYPE CONNECTION_NAME_TYPE;

/// Length (in bytes) of a TS message.
typedef uint32_t MESSAGE_SIZE_TYPE;

/// Link to the data-model type information (message type GUID).
typedef GUID_TYPE MESSAGE_GUID_TYPE;

/// UID scoped to be unique within a system (not globally).
typedef int64_t UID_TYPE;

/// Unique identifier for a TSS connection; obtained from Create_Connection().
typedef UID_TYPE CONNECTION_ID_TYPE;

/// Ties request/reply messages together.
typedef UID_TYPE TRANSACTION_ID_TYPE;

/// Message GUID is provided by the callee of the function (not the caller).
static constexpr GUID_TYPE CALLEE_PROVIDES_GUID = 0;

/// Used to set transaction_id for publisher/subscriber connections.
static constexpr TRANSACTION_ID_TYPE TID_NOT_APPLICABLE = static_cast<TRANSACTION_ID_TYPE>(-1);

/// Used when a client calls Send in client/server connections;
/// the callee assigns the transaction ID.
static constexpr TRANSACTION_ID_TYPE CALLEE_PROVIDES_TID = 0;

//----------------------------------------------------------------------------
// QoS types
//----------------------------------------------------------------------------

/// Single Quality-of-Service key/value pair.
struct QoS_Element {
    STRING_TYPE keyname;
    STRING_TYPE value;
};

/// Sequence of QoS elements passed with each TSS message.
/// Maps to IDL: typedef sequence<QoS_Element> QoS_EVENT_TYPE;
/// Uses std::vector for simplicity; a fixed-capacity alternative may be
/// substituted for ARINC 653 partitions that prohibit dynamic allocation.
typedef std::vector<QoS_Element> QoS_EVENT_TYPE;

//----------------------------------------------------------------------------
// Message header
//----------------------------------------------------------------------------

/// Standard TSS message header carrying instance UID, source UID, and time.
struct HEADER_TYPE {
    UID_TYPE         instance_uid; ///< identifies the message instance
    UID_TYPE         source_uid;   ///< identifies the message source
    SYSTEM_TIME_TYPE timestamp;    ///< nanoseconds since UNIX epoch
};

//----------------------------------------------------------------------------
// Data buffer / message types
//----------------------------------------------------------------------------

/// Size in bytes.
typedef uint32_t BYTE_SIZE_TYPE;

/// Raw data buffer: caller-managed memory at buffer_address with capacity bytes.
struct DATA_BUFFER_TYPE {
    SYSTEM_ADDRESS_TYPE buffer_address;  ///< pointer to the raw buffer
    BYTE_SIZE_TYPE      buffer_capacity; ///< size of the buffer in bytes
};

/// Typed message container: a GUID identifies the message type and
/// DATA_BUFFER_TYPE carries the serialised payload.
struct MESSAGE_TYPE {
    MESSAGE_GUID_TYPE message_guid; ///< identifies the type of the payload
    DATA_BUFFER_TYPE  buffer;       ///< the payload data
};

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_COMMON_HPP
