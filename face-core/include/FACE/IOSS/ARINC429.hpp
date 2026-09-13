// FACE/IOSS/ARINC429.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<ARINC429::ReadWriteBuffer> ARINC429;
// FACE Technical Standard Edition 3.2
//
// ARINC 429 I/O Service – avionics digital data bus (label-based protocol).

#ifndef FACE_IOSS_ARINC429_HPP
#define FACE_IOSS_ARINC429_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace ARINC429 {

// -- Service-specific types -------------------------------------------------

/// Maximum number of 32-bit ARINC 429 labels in a single payload.
static constexpr uint16_t MAX_NUM_LABELS = 65535;

/// Bounded sequence of ARINC 429 32-bit labels
/// (IDL: sequence<unsigned long, MAX_NUM_LABELS>).
struct LABEL_SEQUENCE {
    uint32_t data[MAX_NUM_LABELS];
    uint16_t length; ///< number of valid labels

    LABEL_SEQUENCE() : length(0) {}
};

/// ARINC 429 read/write payload.
struct ReadWriteBuffer {
    LABEL_SEQUENCE label_payload;
};

// -- Parity ------------------------------------------------------------------

typedef uint16_t PARITY_TYPE;
static constexpr PARITY_TYPE PARITY_NONE  = 0;
static constexpr PARITY_TYPE PARITY_ODD   = 1;
static constexpr PARITY_TYPE PARITY_EVEN  = 2;
static constexpr PARITY_TYPE PARITY_MARK  = 3;
static constexpr PARITY_TYPE PARITY_SPACE = 4;

// -- Configuration parameter IDs --------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE DIRECTION     = 0; ///< FACE_BOOLEAN: FALSE=TX, TRUE=RX
static constexpr IO_PARAMETER_ID_TYPE PARITY        = 1; ///< FACE_USHORT: PARITY_TYPE
static constexpr IO_PARAMETER_ID_TYPE CHANNEL_SPEED = 2; ///< FACE_BOOLEAN: FALSE=High, TRUE=Low

// -- Bus status constants ----------------------------------------------------

static constexpr BUS_STATUS_TYPE HW_OPERATIONAL            = 0;
static constexpr BUS_STATUS_TYPE HW_FIFO_OVERFLOW          = 1;
static constexpr BUS_STATUS_TYPE SW_CIRCULAR_BUFF_OVERFLOW = 2;
static constexpr BUS_STATUS_TYPE HW_ADDRESS_ERROR          = 3;
static constexpr BUS_STATUS_TYPE HW_SEQUENCE_ERROR         = 4;
static constexpr BUS_STATUS_TYPE HW_PARITY_ERROR           = 5;
static constexpr BUS_STATUS_TYPE CLOCK_LOSS                = 6;
static constexpr BUS_STATUS_TYPE UNKNOWN_ERROR             = 7;

// -- IO_Service_Module expansion --------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace ARINC429
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_ARINC429_HPP
