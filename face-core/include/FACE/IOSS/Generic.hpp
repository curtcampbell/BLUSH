// FACE/IOSS/Generic.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<Generic::ReadWriteBuffer> Generic;
// FACE Technical Standard Edition 3.2
//
// Generic I/O Service – a byte-buffer based I/O interface for device types
// not covered by the specialised I/O services.
//
// Expanded IDL (abbreviated):
//   module FACE { module IOSS { module Generic {
//     const unsigned short MAX_BYTE_COUNT = 65535;
//     typedef sequence<octet, MAX_BYTE_COUNT> BYTE_SEQUENCE;
//     struct ReadWriteBuffer { BYTE_SEQUENCE data; };
//     // parameter IDs: ADDRESS, MIN_VALUE, MAX_VALUE, INIT_VALUE, PRECISION
//     // IO_Service_Module<ReadWriteBuffer> expansion follows:
//     typedef STRING_TYPE CONNECTION_NAME_TYPE;
//     enum IO_CONNECTION_STATUS_TYPE { ... };
//     ...
//     interface IO_Service { struct READ_PAYLOAD_TYPE { ... }; ... };
//   }; }; };

#ifndef FACE_IOSS_GENERIC_HPP
#define FACE_IOSS_GENERIC_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace Generic {

// -- Service-specific types -------------------------------------------------

/// Maximum number of bytes in a single read/write operation.
static constexpr uint16_t MAX_BYTE_COUNT = 65535;

/// Bounded byte sequence for generic I/O payload.
/// Represented in C++ as a fixed-capacity array with an explicit length field
/// (IDL: sequence<octet, MAX_BYTE_COUNT>).
struct BYTE_SEQUENCE {
    uint8_t  data[MAX_BYTE_COUNT];
    uint16_t length; ///< number of valid bytes in data[]

    BYTE_SEQUENCE() : length(0) {}
};

/// Generic read/write payload buffer.
struct ReadWriteBuffer {
    BYTE_SEQUENCE data;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE ADDRESS   = 0; ///< FACE_ULONGLONG
static constexpr IO_PARAMETER_ID_TYPE MIN_VALUE = 1; ///< FACE_ULONGLONG
static constexpr IO_PARAMETER_ID_TYPE MAX_VALUE = 2; ///< FACE_ULONGLONG
static constexpr IO_PARAMETER_ID_TYPE INIT_VALUE = 3; ///< FACE_ULONGLONG: initial value
static constexpr IO_PARAMETER_ID_TYPE PRECISION  = 4; ///< FACE_LONGDOUBLE

// Note: no defined bus status types for the Generic I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace Generic
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_GENERIC_HPP
