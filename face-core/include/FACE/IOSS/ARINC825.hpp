// FACE/IOSS/ARINC825.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<ARINC825::DataFrameAbstraction> ARINC825;
// FACE Technical Standard Edition 3.2
//
// ARINC 825 I/O Service – portable abstraction of a CAN controller interface.
// Supports Classical CAN and CAN FD (ARINC 825-4).
//
// NOTE: The payload type is DataFrameAbstraction (not ReadWriteBuffer) —
// ARINC 825 uses an abstract CAN Data Frame representation as its payload.

#ifndef FACE_IOSS_ARINC825_HPP
#define FACE_IOSS_ARINC825_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace ARINC825 {

// -- Service-specific types -------------------------------------------------

/// 29-bit CAN Identifier (stored as uint32_t per ARINC 825 Section 5.2).
typedef uint32_t CAN_IDENTIFIER_TYPE;

/// Maximum data bytes for Classical CAN Extended Data Frame.
static constexpr uint8_t CAN_MAX_DLC    = 8;

/// Maximum data bytes for CAN FD Extended Data Frame.
static constexpr uint8_t CAN_FD_MAX_DLC = 64;

/// Bounded CAN data buffer (sized for CAN FD).
/// (IDL: sequence<octet, CAN_FD_MAX_DLC>)
struct CAN_BUFFER_TYPE {
    uint8_t data[CAN_FD_MAX_DLC];
    uint8_t length; ///< number of valid bytes (= DLC)

    CAN_BUFFER_TYPE() : length(0) {}
};

/// Abstract representation of a CAN Extended Data Frame.
///
/// Maps Arbitration Field (id) and Data Field (buffer).
/// Control, CRC, and ACK fields are managed internally by the driver.
struct DataFrameAbstraction {
    CAN_IDENTIFIER_TYPE id;     ///< 29-bit CAN Identifier
    CAN_BUFFER_TYPE     buffer; ///< data payload (DLC = buffer.length)
};

// -- Connection parameter IDs -----------------------------------------------

/// Query FDF bit (CAN FD flag): FALSE=Classical CAN, TRUE=CAN FD.
/// Read-only; Configure_Connection_Parameters returns INVALID_PARAM.
static constexpr IO_PARAMETER_ID_TYPE QUERY_FDF = 0; ///< FACE_BOOLEAN

// -- Bus parameter IDs -------------------------------------------------------

typedef uint8_t CAN_BIT_RATE;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_83_KBPS  = 0;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_125_KBPS = 1;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_250_KBPS = 2;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_500_KBPS = 3;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_1_MBPS   = 4;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_2_MBPS   = 5;
static constexpr CAN_BIT_RATE MAX_CAN_BIT_RATE_4_MBPS   = 6;

/// Query bus bit rate – read-only; Configure_Bus_Parameters returns INVALID_PARAM.
static constexpr IO_PARAMETER_ID_TYPE QUERY_BIT_RATE = 1; ///< FACE_OCTET: CAN_BIT_RATE

// -- Connection fault types (Section 4.5.1) ---------------------------------

typedef uint8_t CAN_FAULT_TYPE;
static constexpr CAN_FAULT_TYPE CAN_FAULT_NONE         = 0;
static constexpr CAN_FAULT_TYPE CAN_FAULT_BIT_ERROR    = 1;
static constexpr CAN_FAULT_TYPE CAN_FAULT_BIT_STUFFING = 2;
static constexpr CAN_FAULT_TYPE CAN_FAULT_CRC_ERROR    = 3;
static constexpr CAN_FAULT_TYPE CAN_FAULT_FORM_ERROR   = 4;
static constexpr CAN_FAULT_TYPE CAN_FAULT_ACK_ERROR    = 5;

/// Query per-connection fault state – read-only.
static constexpr IO_PARAMETER_ID_TYPE QUERY_CAN_FAULT = 2; ///< FACE_OCTET: CAN_FAULT_TYPE

// -- CAN node state machine (Section 4.6) -----------------------------------

typedef uint8_t CAN_NODE_STATE_TYPE;
static constexpr CAN_NODE_STATE_TYPE CAN_NODE_STATE_INIT    = 0;
static constexpr CAN_NODE_STATE_TYPE CAN_NODE_STATE_NORMAL  = 1;
static constexpr CAN_NODE_STATE_TYPE CAN_NODE_STATE_BUS_OFF = 2;

/// Query per-connection CAN node state – read-only.
static constexpr IO_PARAMETER_ID_TYPE QUERY_CAN_NODE_STATE = 3; ///< FACE_OCTET: CAN_NODE_STATE_TYPE

/// Command CAN node reset (sole parameter). Get_Connection_Parameters returns INVALID_PARAM.
static constexpr IO_PARAMETER_ID_TYPE CAN_NODE_RESET = 4; ///< FACE_OCTET

// -- IO_Service_Module expansion (payload = DataFrameAbstraction) -----------

typedef DataFrameAbstraction PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace ARINC825
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_ARINC825_HPP
