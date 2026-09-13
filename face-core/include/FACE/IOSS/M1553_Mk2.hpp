// FACE/IOSS/M1553_Mk2.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<M1553_Mk2::ReadWriteBuffer> M1553_Mk2;
// FACE Technical Standard Edition 3.2
//
// MIL-STD-1553 I/O Service Mark 2 – structured representation of command,
// status, and data words as defined in MIL-STD-1553B.
// References in [] are to MIL-STD-1553B specification.

#ifndef FACE_IOSS_M1553_MK2_HPP
#define FACE_IOSS_M1553_MK2_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace M1553_Mk2 {

// -- Service-specific types -------------------------------------------------

/// Identifies the kind of command word per MIL-STD-1553B Figures 6 & 7.
enum COMMAND_WORD_KIND {
    RECEIVE,   ///< Receive Command (T/R = logical zero)
    TRANSMIT,  ///< Transmit Command (T/R = logical one)
    MODE,      ///< Assign T/R and mode_code per Table 1
    IGNORED    ///< Not used (cw2 in non-RT-to-RT transfers)
};

/// Logical representation of a Command Word per [4.3.3.5.1].
struct CommandWordData {
    COMMAND_WORD_KIND cw_kind;
    uint8_t rt_addr;   ///< Remote Terminal Address (0-31)   [4.3.3.5.1.2]
    uint8_t subaddr;   ///< Sub Address Number (0-31)         [4.3.3.5.1.4]
    uint8_t mode_code; ///< 255 if not a mode command; else mode code [4.3.3.5.1.7]
};

/// Logical representation of a Status Word per [4.3.3.5.3].
struct StatusWordData {
    uint8_t rt_addr;          ///< Remote Terminal Address (0-31)  [4.3.3.5.3.2]
    bool    message_error;    ///< [4.3.3.5.3.3]
    bool    instrumentation;  ///< [4.3.3.5.3.4]
    bool    service_request;  ///< [4.3.3.5.3.5]
    uint8_t reserved_bits;    ///< [4.3.3.5.3.6]
    bool    bcast_cmd_rcvd;   ///< [4.3.3.5.3.7]
    bool    busy;             ///< [4.3.3.5.3.8]
    bool    subsystem_flag;   ///< [4.3.3.5.3.9]
    bool    dynamic_bus_ctrl; ///< [4.3.3.5.3.10]
    bool    terminal_flag;    ///< [4.3.3.5.3.11]
};

typedef uint16_t DATA_WORD_TYPE;
static constexpr uint8_t MAX_DATA_WORD_COUNT = 32;

/// Bounded sequence of MIL-STD-1553 data words
/// (IDL: sequence<DATA_WORD_TYPE, MAX_DATA_WORD_COUNT>).
struct DATA_WORDS {
    DATA_WORD_TYPE data[MAX_DATA_WORD_COUNT];
    uint8_t        length; ///< number of valid data words (= DLC)

    DATA_WORDS() : length(0) {}
};

/// Read/write buffer covering all information transfer formats from
/// MIL-STD-1553B Figure 6 and Figure 7.
///
///   Controller to RT:  cw1.cw_kind = RECEIVE;  cw2.cw_kind = IGNORED
///   RT to Controller:  cw1.cw_kind = TRANSMIT; cw2.cw_kind = IGNORED
///   RT to RT:          cw1.cw_kind = RECEIVE;  cw2.cw_kind = TRANSMIT
///   Mode Cmd no data:  cw1.cw_kind = MODE;     cw2.cw_kind = IGNORED
///   Mode Cmd Tx data:  cw1.cw_kind = MODE;     cw2.cw_kind = TRANSMIT
///   Mode Cmd Rx data:  cw1.cw_kind = MODE;     cw2.cw_kind = RECEIVE
struct ReadWriteBuffer {
    uint8_t         bus_id; ///< identifies the 1553 bus (1-15)
    CommandWordData cw1;    ///< primary command word
    StatusWordData  sw1;    ///< primary status word
    CommandWordData cw2;    ///< second command word (RT-to-RT)
    StatusWordData  sw2;    ///< second status word  (RT-to-RT)
    DATA_WORDS      data;   ///< data words; length = data word count per [4.3.3.5.1.5]
};

// -- Channel mode flags (combinable bitmask) --------------------------------

typedef uint16_t CHANNEL_MODE_TYPE;
static constexpr CHANNEL_MODE_TYPE BC       = 0x0001; ///< Bus Controller
static constexpr CHANNEL_MODE_TYPE BBC      = 0x0002; ///< Backup Bus Controller
static constexpr CHANNEL_MODE_TYPE RT       = 0x0004; ///< Remote Terminal
static constexpr CHANNEL_MODE_TYPE BM       = 0x0008; ///< Bus Monitor
static constexpr CHANNEL_MODE_TYPE MULTI_RT = 0x0010; ///< Multi-Function Terminal

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE CHANNEL_MODE        = 1; ///< FACE_USHORT: CHANNEL_MODE_TYPE
static constexpr IO_PARAMETER_ID_TYPE TERMINAL_ADDRESS    = 2; ///< FACE_OCTET
static constexpr IO_PARAMETER_ID_TYPE TERMINAL_SUBADDRESS = 3; ///< FACE_OCTET
static constexpr IO_PARAMETER_ID_TYPE CHANNEL_NUM_BUSES   = 4; ///< FACE_OCTET

// -- Bus status constants (encode bus identifier in high nibble) ------------

static constexpr BUS_STATUS_TYPE DRIVER_ERROR = 0xDEAD;

/// OR into status to indicate which bus is being reported.
static constexpr BUS_STATUS_TYPE BUS_A_REPORTED = 0x1000;
static constexpr BUS_STATUS_TYPE BUS_B_REPORTED = 0x2000;
static constexpr BUS_STATUS_TYPE BUS_C_REPORTED = 0x3000;
static constexpr BUS_STATUS_TYPE BUS_D_REPORTED = 0x4000;

/// Per-bus error flags (OR with bus-reported bits).
static constexpr BUS_STATUS_TYPE REPORTED_BUS_SEQUENCING_ERROR = 0x0001; ///< [4.3.3.6]
static constexpr BUS_STATUS_TYPE REPORTED_BUS_TIMING_ERROR     = 0x0002; ///< [4.3.3.7/4.3.3.8]
static constexpr BUS_STATUS_TYPE REPORTED_BUS_NO_RESPONSE      = 0x0004; ///< [4.3.3.9]
static constexpr BUS_STATUS_TYPE REPORTED_BUS_ENCODING_ERROR   = 0x0008; ///< [4.4.1/4.4.2]
static constexpr BUS_STATUS_TYPE REPORTED_BUS_ILLEGAL_CMD      = 0x0010; ///< [4.4.3.4]

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace M1553_Mk2
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_M1553_MK2_HPP
