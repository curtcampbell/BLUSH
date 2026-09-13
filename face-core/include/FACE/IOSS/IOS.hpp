// FACE/IOSS/IOS.hpp
// C++ binding of module-level declarations from FACE::IOSS (IOS.idl)
// FACE Technical Standard Edition 3.2
//
// IDL source (module-level, outside IO_Service_Module<>):
//   module FACE { module IOSS {
//     typedef unsigned short BUS_STATUS_TYPE;
//     typedef unsigned short IO_PARAMETER_ID_TYPE;
//     typedef long long CONNECTION_HANDLE_TYPE;
//     const CONNECTION_HANDLE_TYPE INVALID_CONNECTION_HANDLE = -1;
//     const CONNECTION_HANDLE_TYPE IGNORED_CONNECTION_HANDLE =  0;
//   }; };
//
// The IO_Service_Module<PAYLOAD_DATA_MSG_TYPE> template body is NOT
// rendered here.  It is instead expanded textually for each I/O Service
// by including detail/IO_Service_Expansion.inc inside the respective
// service namespace after typedef-ing PAYLOAD_DATA_MSG_TYPE.
//
// See: FACE Technical Standard Edition 3.2, Section 7.4 (IOSS)

#ifndef FACE_IOSS_IOS_HPP
#define FACE_IOSS_IOS_HPP

#include "../Common.hpp"
#include <cstdint>
#include <vector>

namespace FACE {
namespace IOSS {

/// Status of the I/O device bus.
/// Bus-specific status constants are defined in each service namespace.
typedef uint16_t BUS_STATUS_TYPE;

/// Identifier used to programmatically query or configure I/O parameters.
/// Bus-specific parameter ID constants are defined in each service namespace.
typedef uint16_t IO_PARAMETER_ID_TYPE;

/// Opaque handle identifying an I/O connection (returned by Open_Connection).
typedef int64_t CONNECTION_HANDLE_TYPE;

/// Returned by Open_Connection when the connection could not be established.
static constexpr CONNECTION_HANDLE_TYPE INVALID_CONNECTION_HANDLE =
    static_cast<CONNECTION_HANDLE_TYPE>(-1);

/// Passed as handle for bus-level operations not tied to any specific connection.
static constexpr CONNECTION_HANDLE_TYPE IGNORED_CONNECTION_HANDLE =
    static_cast<CONNECTION_HANDLE_TYPE>(0);

} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_IOS_HPP
