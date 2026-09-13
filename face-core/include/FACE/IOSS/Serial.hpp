// FACE/IOSS/Serial.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<Serial::ReadWriteBuffer> Serial;
// FACE Technical Standard Edition 3.2
//
// Serial I/O Service – RS-232/422/485 and similar serial interfaces.

#ifndef FACE_IOSS_SERIAL_HPP
#define FACE_IOSS_SERIAL_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace Serial {

// -- Service-specific types -------------------------------------------------

/// Maximum bytes in a serial read/write buffer.
static constexpr uint16_t MAX_BYTE_COUNT = 65535;

/// Bounded byte sequence for serial data
/// (IDL: sequence<octet, MAX_BYTE_COUNT>).
struct DATA_BUFFER_TYPE {
    uint8_t  data[MAX_BYTE_COUNT];
    uint16_t length; ///< number of valid bytes

    DATA_BUFFER_TYPE() : length(0) {}
};

/// Serial read/write payload.
struct ReadWriteBuffer {
    uint8_t          channel; ///< channel on which data is transmitted/received
    DATA_BUFFER_TYPE data;    ///< serial data bytes
};

// -- Operational modes -------------------------------------------------------

typedef int16_t OPERATIONAL_MODE_TYPE;
static constexpr OPERATIONAL_MODE_TYPE RS_232 = 0;
static constexpr OPERATIONAL_MODE_TYPE RS_422 = 1;
static constexpr OPERATIONAL_MODE_TYPE RS_485 = 2;

// -- Flow control ------------------------------------------------------------

typedef uint16_t FLOW_CONTROL_TYPE;
static constexpr FLOW_CONTROL_TYPE NONE     = 0;
static constexpr FLOW_CONTROL_TYPE XON_XOFF = 1;
static constexpr FLOW_CONTROL_TYPE RTS_CTS  = 2;
static constexpr FLOW_CONTROL_TYPE DSR_DTR  = 3;

// -- Parity ------------------------------------------------------------------

typedef uint16_t PARITY_TYPE;
static constexpr PARITY_TYPE PARITY_NONE  = 0;
static constexpr PARITY_TYPE PARITY_ODD   = 1;
static constexpr PARITY_TYPE PARITY_EVEN  = 2;
static constexpr PARITY_TYPE PARITY_MARK  = 3;
static constexpr PARITY_TYPE PARITY_SPACE = 4;

// -- Configuration parameter IDs --------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE CHANNEL_NUM  = 0; ///< FACE_OCTET: physical port number
static constexpr IO_PARAMETER_ID_TYPE MODE         = 1; ///< FACE_SHORT: OPERATIONAL_MODE_TYPE
static constexpr IO_PARAMETER_ID_TYPE BAUD_RATE    = 2; ///< FACE_LONG: baud rate
static constexpr IO_PARAMETER_ID_TYPE DATA_BITS    = 3; ///< FACE_SHORT: number of data bits
static constexpr IO_PARAMETER_ID_TYPE STOP_BITS    = 4; ///< FACE_SHORT: number of stop bits
static constexpr IO_PARAMETER_ID_TYPE PARITY       = 5; ///< FACE_USHORT: PARITY_TYPE
static constexpr IO_PARAMETER_ID_TYPE FLOW_CONTROL = 6; ///< FACE_USHORT: FLOW_CONTROL_TYPE

// -- Bus status constants ----------------------------------------------------

static constexpr BUS_STATUS_TYPE DEVICE_OPERATIONAL = 0;
static constexpr BUS_STATUS_TYPE OVERRUN_ERROR      = 1;
static constexpr BUS_STATUS_TYPE PARITY_ERROR       = 2;
static constexpr BUS_STATUS_TYPE FRAMING_ERROR      = 3;
static constexpr BUS_STATUS_TYPE BREAK_ERROR        = 4;

// -- IO_Service_Module expansion --------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace Serial
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_SERIAL_HPP
