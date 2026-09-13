// FACE/Common.hpp
// C++ mapping of FACE/Common.idl
// FACE Technical Standard Edition 3.2
//
// IDL module FACE maps to namespace FACE.
// IDL typedefs, enums, structs, and consts are mapped directly.
// SYSTEM_ADDRESS_TYPE (native) maps to void*.

#ifndef FACE_COMMON_HPP
#define FACE_COMMON_HPP

#include <cstdint>
#include <cstring>  // for std::memcpy, std::memset
#include <cstddef>  // for std::size_t

namespace FACE {

//----------------------------------------------------------------------------
// RETURN_CODE_TYPE
// Status codes returned by FACE API methods.
// The first set (through TIMED_OUT) matches ARINC 653 Part 1, Section 3.4.1.
//----------------------------------------------------------------------------
enum RETURN_CODE_TYPE {
    NO_ERROR,               ///< request valid and operation performed
    NO_ACTION,              ///< status of system unaffected by request
    NOT_AVAILABLE,          ///< no message was available
    INVALID_PARAM,          ///< invalid parameter specified in request
    INVALID_CONFIG,         ///< parameter incompatible with configuration
    INVALID_MODE,           ///< request incompatible with current mode
    TIMED_OUT,              ///< time expired before request could be fulfilled
    ADDR_IN_USE,            ///< address currently in use
    PERMISSION_DENIED,      ///< no permission to send or connecting to wrong partition
    MESSAGE_STALE,          ///< current time - timestamp exceeds configured limits
    IN_PROGRESS,            ///< asynchronous connection in progress
    CONNECTION_CLOSED,      ///< connection was closed
    DATA_BUFFER_TOO_SMALL,  ///< data buffer was too small for message
    DATA_OVERFLOW,          ///< a loss of messages due to data buffer overflow
    RESOURCE_LIMIT_REACHED  ///< the maximum number of resources has been used
};

//----------------------------------------------------------------------------
// Time Types
// Based on ARINC 653 Part 1, Section 3.4.1. Resolution: 1 nanosecond.
//----------------------------------------------------------------------------

/// Duration; corresponds to ARINC 653 duration (nanoseconds).
typedef int64_t DURATION_TIME_TYPE;

/// Absolute time; UNIX epoch (nanoseconds since 1970-01-01T00:00:00Z).
typedef int64_t ABSOLUTE_TIME_TYPE;

/// System time; 64-bit signed integer with 1 ns resolution.
typedef ABSOLUTE_TIME_TYPE SYSTEM_TIME_TYPE;

/// Timeout; 1 nanosecond resolution.
typedef DURATION_TIME_TYPE TIMEOUT_TYPE;

/// Sentinel value indicating an indefinitely long wait (no timeout).
static constexpr TIMEOUT_TYPE INF_TIME_VALUE = static_cast<TIMEOUT_TYPE>(-1);

//----------------------------------------------------------------------------
// String Types
//----------------------------------------------------------------------------

/// Maximum length of a bounded STRING_TYPE (256 characters including NUL).
static constexpr std::size_t STRING_TYPE_MAX_LENGTH = 256;

/// Bounded string of up to 255 characters (+ NUL terminator).
/// Corresponds to IDL: typedef string<256> STRING_TYPE;
struct STRING_TYPE {
    char data[STRING_TYPE_MAX_LENGTH];

    STRING_TYPE() { std::memset(data, 0, sizeof(data)); }

    explicit STRING_TYPE(const char* s) {
        std::memset(data, 0, sizeof(data));
        if (s) {
            std::size_t len = 0;
            while (s[len] && len < STRING_TYPE_MAX_LENGTH - 1) { ++len; }
            std::memcpy(data, s, len);
        }
    }

    const char* c_str() const { return data; }

    bool operator==(const STRING_TYPE& rhs) const {
        return std::memcmp(data, rhs.data, STRING_TYPE_MAX_LENGTH) == 0;
    }
    bool operator!=(const STRING_TYPE& rhs) const { return !(*this == rhs); }
};

/// Unbounded string type.
/// In C++ we typedef to const char* for lightweight contexts.
/// Implementations may substitute std::string if the target allows dynamic memory.
typedef const char* UNBOUNDED_STRING_TYPE;

/// Configuration resource locator (file name, URI, or service reference).
typedef STRING_TYPE CONFIGURATION_RESOURCE;

//----------------------------------------------------------------------------
// Address and GUID Types
//----------------------------------------------------------------------------

/// Opaque system address (maps to IDL native SYSTEM_ADDRESS_TYPE).
typedef void* SYSTEM_ADDRESS_TYPE;

/// Globally Unique Identifier.
typedef int64_t GUID_TYPE;

} // namespace FACE

#endif // FACE_COMMON_HPP
