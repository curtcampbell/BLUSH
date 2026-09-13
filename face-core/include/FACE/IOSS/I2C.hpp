// FACE/IOSS/I2C.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<I2C::MASTER_COMMAND_TYPE> I2C;
//   (plus the Combined_RW_IO_Service extension)
// FACE Technical Standard Edition 3.2
//
// I2C I/O Service – Inter-Integrated Circuit (I2C) bus interface.
// Supports 7-bit, 10-bit, and 16-bit slave addressing, and adds a
// Combined_RW_IO_Service that performs atomic combined read/write operations.
//
// NOTE: The payload type is MASTER_COMMAND_TYPE (not ReadWriteBuffer).
// NOTE: The Injectable IDL uses Combined_RW_IO_Service as the injected type,
//       not IO_Service — see I2C_Injectable.hpp.

#ifndef FACE_IOSS_I2C_HPP
#define FACE_IOSS_I2C_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace I2C {

// -- Service-specific types -------------------------------------------------

typedef uint16_t SLAVE_ADDRESS_TYPE;
typedef uint16_t SLAVE_ADDRESS_SIZE_TYPE;

/// Pointer to a caller-managed message buffer (maps to IDL native
/// SYSTEM_ADDRESS_TYPE).
typedef SYSTEM_ADDRESS_TYPE MESSAGE_ADDR_TYPE;

static constexpr SLAVE_ADDRESS_SIZE_TYPE SLAVE_ADDRESS_SIZE_7  = 0;
static constexpr SLAVE_ADDRESS_SIZE_TYPE SLAVE_ADDRESS_SIZE_10 = 1;
static constexpr SLAVE_ADDRESS_SIZE_TYPE SLAVE_ADDRESS_SIZE_16 = 2;

/// Master I2C transaction descriptor – the payload type for Read/Write.
///
/// For slave read:  if slave buffer address != slave RX buffer address,
///                  slave RX buffer address is reset.
/// For slave write: if slave buffer address != slave TX buffer address,
///                  slave TX buffer address is reset.
struct MASTER_COMMAND_TYPE {
    SLAVE_ADDRESS_SIZE_TYPE slave_address_size;
    SLAVE_ADDRESS_TYPE      slave_address;
    uint16_t                message_length;
    MESSAGE_ADDR_TYPE       data_buffer_address;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE IS_MASTER         = 0; ///< FACE_BOOLEAN: FALSE=slave, TRUE=master
static constexpr IO_PARAMETER_ID_TYPE BAUD              = 1; ///< FACE_LONG: baud rate
static constexpr IO_PARAMETER_ID_TYPE MY_ADDRESS        = 2; ///< FACE_USHORT: SLAVE_ADDRESS_TYPE
static constexpr IO_PARAMETER_ID_TYPE RX_BUFFER_ADDRESS = 3; ///< MESSAGE_ADDR_TYPE (native)
static constexpr IO_PARAMETER_ID_TYPE RX_BUFFER_LENGTH  = 4; ///< FACE_ULONG
static constexpr IO_PARAMETER_ID_TYPE TX_BUFFER_ADDRESS = 5; ///< MESSAGE_ADDR_TYPE (native)
static constexpr IO_PARAMETER_ID_TYPE TX_BUFFER_LENGTH  = 6; ///< FACE_ULONG

// -- Bus status constants ---------------------------------------------------

static constexpr BUS_STATUS_TYPE DEVICE_OPERATIONAL = 0;
static constexpr BUS_STATUS_TYPE OVERRUN_ERROR      = 1;
static constexpr BUS_STATUS_TYPE PARITY_ERROR       = 2;
static constexpr BUS_STATUS_TYPE FRAMING_ERROR      = 3;
static constexpr BUS_STATUS_TYPE ADDRESS_ERROR      = 4;

// -- IO_Service_Module expansion (payload = MASTER_COMMAND_TYPE) -----------
// This defines IO_Service with nested READ_PAYLOAD_TYPE { MASTER_COMMAND_TYPE payload; }
// and all other IO_Service_Module types.

typedef MASTER_COMMAND_TYPE PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

// -- Combined_RW_IO_Service extension ---------------------------------------
// Additional IDL inside module I2C (after the IO_Service_Module instantiation):
//   enum COMMAND_KIND_TYPE { READ, WRITE };
//   struct ATOMIC_IO_DEF_ENTRY_TYPE { ... };
//   typedef sequence<ATOMIC_IO_DEF_ENTRY_TYPE> MASTER_COMMANDS_TYPE;
//   interface Combined_RW_IO_Service : I2C::IO_Service {
//     void Perform_Combined_Commands(...);
//   };

/// Command kind for atomic combined I/O operations.
enum COMMAND_KIND_TYPE {
    READ,
    WRITE
};

/// Single entry in a combined read/write command sequence.
struct ATOMIC_IO_DEF_ENTRY_TYPE {
    COMMAND_KIND_TYPE   cmd;
    MASTER_COMMAND_TYPE master_command;
};

/// Ordered sequence of atomic I/O commands.
typedef std::vector<ATOMIC_IO_DEF_ENTRY_TYPE> MASTER_COMMANDS_TYPE;

/// Extended I2C I/O Service with atomic combined read/write capability.
///
/// Inherits the standard IO_Service and adds Perform_Combined_Commands.
class Combined_RW_IO_Service : public IO_Service {
public:
    virtual ~Combined_RW_IO_Service() {}

    /// Perform a sequence of atomic combined I/O commands on a single connection.
    ///
    /// @param handle       The open connection handle.
    /// @param timeout      Maximum time to wait; INF_TIME_VALUE blocks indefinitely.
    /// @param payload      The ordered sequence of read/write commands to execute.
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Perform_Combined_Commands(
        CONNECTION_HANDLE_TYPE handle,
        TIMEOUT_TYPE           timeout,
        MASTER_COMMANDS_TYPE&  payload,
        RETURN_CODE_TYPE&      return_code) = 0;
};

} // namespace I2C
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_I2C_HPP
