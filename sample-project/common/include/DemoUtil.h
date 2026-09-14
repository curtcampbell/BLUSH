#pragma once

// DemoUtil.h
//
// Tiny console-logging helpers shared by the sample-project UoP impls and
// the integration executable. Not a face-utils component -- it exists only
// to make this demo's output legible.

#include "FACE/Common.hpp"

#include <cstdio>
#include <string>

namespace SampleProject {

/// Human-readable name for a FACE::RETURN_CODE_TYPE, for demo logging only.
inline const char* RcName(FACE::RETURN_CODE_TYPE rc)
{
    switch (rc) {
        case FACE::NO_ERROR:              return "NO_ERROR";
        case FACE::NO_ACTION:             return "NO_ACTION";
        case FACE::NOT_AVAILABLE:         return "NOT_AVAILABLE";
        case FACE::INVALID_PARAM:         return "INVALID_PARAM";
        case FACE::INVALID_CONFIG:        return "INVALID_CONFIG";
        case FACE::INVALID_MODE:          return "INVALID_MODE";
        case FACE::TIMED_OUT:             return "TIMED_OUT";
        case FACE::ADDR_IN_USE:           return "ADDR_IN_USE";
        case FACE::PERMISSION_DENIED:     return "PERMISSION_DENIED";
        case FACE::MESSAGE_STALE:         return "MESSAGE_STALE";
        case FACE::IN_PROGRESS:           return "IN_PROGRESS";
        case FACE::CONNECTION_CLOSED:     return "CONNECTION_CLOSED";
        case FACE::DATA_BUFFER_TOO_SMALL: return "DATA_BUFFER_TOO_SMALL";
        case FACE::DATA_OVERFLOW:         return "DATA_OVERFLOW";
        case FACE::RESOURCE_LIMIT_REACHED:return "RESOURCE_LIMIT_REACHED";
        default:                          return "<unknown>";
    }
}

/// Logs the outcome of a send___() call. No real TSS is injected anywhere
/// in this sample, so every connection reports NOT_AVAILABLE -- that is
/// the correct, documented behavior of an unwired connection, not a
/// failure of the demo.
inline void LogSend(const std::string& uop, const std::string& connection, FACE::RETURN_CODE_TYPE rc)
{
    std::printf("  [%s] send %-28s -> %s\n", uop.c_str(), connection.c_str(), RcName(rc));
}

/// Logs the outcome of a register___Handler() call. With no TSS injected,
/// the underlying connection is never wired, so the returned handle is
/// always null -- again, documented behavior, not an error.
template <typename RegistrationPtr>
inline void LogRegister(const std::string& uop, const std::string& connection, const RegistrationPtr& handle)
{
    std::printf("  [%s] register %-24s -> %s\n", uop.c_str(), connection.c_str(),
                handle ? "wired" : "nullptr (connection not wired)");
}

} // namespace SampleProject
