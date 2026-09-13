// FACE/TSS/Message_Type_Utilities.hpp
// C++ mapping of FACE/TSS/Message_Type_Utilities.idl
// FACE Technical Standard Edition 3.2
//
// Utilities for allocating, copying, and releasing TSS MESSAGE_TYPE
// buffers aligned to a specific data-view type.

#ifndef FACE_TSS_MESSAGE_TYPE_UTILITIES_HPP
#define FACE_TSS_MESSAGE_TYPE_UTILITIES_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {

//----------------------------------------------------------------------------
// Message_Type_Utility
//----------------------------------------------------------------------------

/// Per-message-type memory management utility.
///
/// Create() allocates a MESSAGE_TYPE buffer aligned to the data-view type.
/// Copy() deep-copies a source buffer.
/// Destroy() reclaims any allocated memory.
///
/// If GUID is set to CALLEE_PROVIDES_GUID, Create() assigns a valid GUID
/// and formats the buffer; otherwise the provided GUID is used.
/// Copy() may resize a shallow allocation.
class Message_Type_Utility {
public:
    virtual ~Message_Type_Utility() {}

    /// Allocate a MESSAGE_TYPE buffer aligned for the data-view type.
    ///
    /// @param message_buffer  Inout: on entry the GUID specifies the type
    ///                        (or CALLEE_PROVIDES_GUID); on success the buffer
    ///                        is populated and the GUID is valid.
    /// @param return_code     Output: NO_ERROR on success.
    virtual void Create(
        MESSAGE_TYPE&     message_buffer,
        RETURN_CODE_TYPE& return_code) = 0;

    /// Deep-copy a source buffer into a pre-created destination buffer.
    ///
    /// The destination must be created with Create() before calling Copy(),
    /// with dest_buffer.message_guid == CALLEE_PROVIDES_GUID.
    ///
    /// @param src_buffer   Source message to copy.
    /// @param dest_buffer  Inout: destination; receives a deep copy of src.
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Copy(
        const MESSAGE_TYPE& src_buffer,
        MESSAGE_TYPE&       dest_buffer,
        RETURN_CODE_TYPE&   return_code) = 0;

    /// Release memory and clean up a previously created message buffer.
    ///
    /// @param message_buffer  Inout: buffer to destroy; zeroed on success.
    /// @param return_code     Output: NO_ERROR on success.
    virtual void Destroy(
        MESSAGE_TYPE&     message_buffer,
        RETURN_CODE_TYPE& return_code) = 0;
};

//----------------------------------------------------------------------------
// MT_Utility_IF_Lookup
//----------------------------------------------------------------------------

/// Factory for retrieving per-message-type memory-management helpers.
///
/// The TS calls Get_Message_Helper() to obtain a Message_Type_Utility
/// instance bound to a specific message type GUID.
class MT_Utility_IF_Lookup {
public:
    virtual ~MT_Utility_IF_Lookup() {}

    /// Retrieve the Message_Type_Utility for a message type.
    ///
    /// @param message_type_id  GUID identifying the message type.
    /// @param message_helper   Output: pointer to the utility implementation.
    /// @param return_code      Output: NO_ERROR if the type is known;
    ///                         INVALID_PARAM if the GUID is unrecognised.
    virtual void Get_Message_Helper(
        GUID_TYPE               message_type_id,
        Message_Type_Utility*&  message_helper,
        RETURN_CODE_TYPE&       return_code) = 0;
};

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_MESSAGE_TYPE_UTILITIES_HPP
