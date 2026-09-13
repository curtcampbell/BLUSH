// FACE/IOSS/M1553.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<M1553::ReadWriteBuffer> M1553;
// FACE Technical Standard Edition 3.2
//
// MIL-STD-1553 I/O Service.

#ifndef FACE_IOSS_M1553_HPP
#define FACE_IOSS_M1553_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace M1553 {

// -- Service-specific types -------------------------------------------------

/// MIL-STD-1553 transfer direction.
enum TRANSMISSION_TYPE {
    RECEIVE,
    TRANSMIT
};

/// MIL-STD-1553 word type tags.
typedef uint16_t WORD_TYPE;
static constexpr WORD_TYPE CMD    = 0;
static constexpr WORD_TYPE STATUS = 1;
static constexpr WORD_TYPE DATA   = 2;

/// Maximum data words per MIL-STD-1553B transaction.
static constexpr uint8_t MAX_WORD_COUNT = 32;

/// Bounded sequence of MIL-STD-1553 words
/// (IDL: sequence<WORD_TYPE, MAX_WORD_COUNT>).
struct DATA_BUFFER_TYPE {
    WORD_TYPE data[MAX_WORD_COUNT];
    uint8_t   length; ///< number of valid words

    DATA_BUFFER_TYPE() : length(0) {}
};

/// MIL-STD-1553 read/write payload.
struct ReadWriteBuffer {
    uint8_t           bus_id;      ///< identifies the 1553 bus (0-31)
    uint8_t           rt_number_1; ///< Remote Terminal Address (0-31)
    uint8_t           sa_number_1; ///< Sub Address Number (0-31)
    uint8_t           rt_number_2; ///< RT Address for RT-to-RT (0-31)
    uint8_t           sa_number_2; ///< SA Number for RT-to-RT (0-31)
    TRANSMISSION_TYPE t_r;
    DATA_BUFFER_TYPE  data;
};

// -- Channel modes ----------------------------------------------------------

typedef uint16_t CHANNEL_MODE_TYPE;
static constexpr CHANNEL_MODE_TYPE BC  = 0; ///< Bus Controller
static constexpr CHANNEL_MODE_TYPE BBC = 1; ///< Backup Bus Controller
static constexpr CHANNEL_MODE_TYPE RT  = 2; ///< Remote Terminal
static constexpr CHANNEL_MODE_TYPE BM  = 3; ///< Bus Monitor

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE CHANNEL_NUM                = 0; ///< FACE_LONGLONG
static constexpr IO_PARAMETER_ID_TYPE CHANNEL_MODE               = 1; ///< FACE_USHORT: CHANNEL_MODE_TYPE
static constexpr IO_PARAMETER_ID_TYPE RELEASE_BUS_CONTROL_STATE  = 2; ///< FACE_BOOLEAN
static constexpr IO_PARAMETER_ID_TYPE CONFIGURED_TERMINAL_ADDRESS = 3; ///< FACE_ULONG

// -- Bus status constants ---------------------------------------------------

static constexpr BUS_STATUS_TYPE BC_IO_NO_RESPONSE               = 0;
static constexpr BUS_STATUS_TYPE BC_IO_LOOP_TEST_FAIL            = 1;
static constexpr BUS_STATUS_TYPE BC_IO_MSG_RETRIED               = 2;
static constexpr BUS_STATUS_TYPE BC_IO_BAD_DATA_BLOCK            = 3;
static constexpr BUS_STATUS_TYPE BC_IO_ADDRESS_ERROR             = 4;
static constexpr BUS_STATUS_TYPE BC_IO_WORD_COUNT_ERROR          = 5;
static constexpr BUS_STATUS_TYPE BC_IO_SYNC_ERROR                = 6;
static constexpr BUS_STATUS_TYPE BC_IO_INVALID_WORD              = 7;
static constexpr BUS_STATUS_TYPE RT_IO_TERMINAL_FLAG             = 8;
static constexpr BUS_STATUS_TYPE RT_IO_SUBSYSTEM_FLAG            = 9;
static constexpr BUS_STATUS_TYPE RT_IO_SERVICE_REQUEST           = 10;
static constexpr BUS_STATUS_TYPE RT_IO_BUSY                      = 11;
static constexpr BUS_STATUS_TYPE RT_IO_DYNAMIC_BC                = 12;
static constexpr BUS_STATUS_TYPE RT_IO_NO_RESPONSE               = 13;
static constexpr BUS_STATUS_TYPE RT_IO_LOOP_TEST_FAIL            = 14;
static constexpr BUS_STATUS_TYPE RT_IO_ILLEGAL_COMMAND_WORD      = 15;
static constexpr BUS_STATUS_TYPE RT_IO_WORD_COUNT_ERROR          = 16;
static constexpr BUS_STATUS_TYPE RT_IO_SYNC_ERROR                = 17;
static constexpr BUS_STATUS_TYPE RT_IO_INVALID_WORD              = 18;
static constexpr BUS_STATUS_TYPE RT_IO_RT_RT_GAP_SYNC_ADDR_ERROR = 19;
static constexpr BUS_STATUS_TYPE RT_IO_RT_RT_2ND_CMD_ERROR       = 20;
static constexpr BUS_STATUS_TYPE RT_IO_COMMAND_WORD_ERROR        = 21;
static constexpr BUS_STATUS_TYPE BM_IO_NO_RESPONSE               = 22;
static constexpr BUS_STATUS_TYPE BM_IO_WORD_COUNT_ERROR          = 23;
static constexpr BUS_STATUS_TYPE BM_IO_SYNC_ERROR                = 24;
static constexpr BUS_STATUS_TYPE BM_IO_INVALID_WORD              = 25;
static constexpr BUS_STATUS_TYPE BM_IO_RT_RT_GAP_SYNC_ADDR_ERROR = 26;
static constexpr BUS_STATUS_TYPE BM_IO_RT_RT_2ND_CMD_ERROR       = 27;
static constexpr BUS_STATUS_TYPE BM_IO_COMMAND_WORD_ERROR        = 28;
static constexpr BUS_STATUS_TYPE BM_IO_BAD_DATA_BLOCK            = 29;
static constexpr BUS_STATUS_TYPE BM_IO_MESSAGE_ERROR             = 30;
static constexpr BUS_STATUS_TYPE BM_IO_INSTRUMENTATION           = 31;
static constexpr BUS_STATUS_TYPE BM_IO_SERVICE_REQUEST           = 32;
static constexpr BUS_STATUS_TYPE BM_IO_RESERVED_BITS             = 33;
static constexpr BUS_STATUS_TYPE BM_IO_BROADCAST_RCVD            = 34;
static constexpr BUS_STATUS_TYPE BM_IO_BUSY                      = 35;
static constexpr BUS_STATUS_TYPE BM_IO_SF                        = 36;
static constexpr BUS_STATUS_TYPE BM_IO_DYNAMIC_BC                = 37;
static constexpr BUS_STATUS_TYPE BM_IO_TF                        = 38;
static constexpr BUS_STATUS_TYPE DRIVER_READY                    = 39;
static constexpr BUS_STATUS_TYPE DRIVER_ERROR                    = 40;
static constexpr BUS_STATUS_TYPE UNKNOWN_ERROR                   = 41;
static constexpr BUS_STATUS_TYPE RX_SUCCESS                      = 42;
static constexpr BUS_STATUS_TYPE TX_SUCCESS                      = 43;
static constexpr BUS_STATUS_TYPE RXMODE_SUCCESS                  = 44;
static constexpr BUS_STATUS_TYPE TXMODE_SUCCESS                  = 45;
static constexpr BUS_STATUS_TYPE RT_TO_RT_SUCCESS                = 46;
static constexpr BUS_STATUS_TYPE BC_IO_GO                        = 47;
static constexpr BUS_STATUS_TYPE BC_IO_NOGO_A                    = 48;
static constexpr BUS_STATUS_TYPE BC_IO_NOGO_B                    = 49;
static constexpr BUS_STATUS_TYPE BC_IO_NOGO_T                    = 50;

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace M1553
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_M1553_HPP
