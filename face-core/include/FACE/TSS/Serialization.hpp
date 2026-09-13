// FACE/TSS/Serialization.hpp
// C++ mapping of FACE/TSS/Serialization.idl
// FACE Technical Standard Edition 3.2
//
// Message serialisation interfaces used by the TPM to encode/decode
// typed messages to/from raw wire buffers.

#ifndef FACE_TSS_SERIALIZATION_HPP
#define FACE_TSS_SERIALIZATION_HPP

#include "Common.hpp"
#include "Primitive_Marshalling.hpp"

namespace FACE {
namespace TSS {

//----------------------------------------------------------------------------
// Message_Serialization
//----------------------------------------------------------------------------

/// Per-message-type serialisation interface.
///
/// Each message type registered with the TS has a corresponding
/// Message_Serialization instance that knows how to encode and decode
/// that type to/from a raw byte buffer.
class Message_Serialization {
public:
    virtual ~Message_Serialization() {}

    /// Serialise a typed message into a raw byte buffer.
    ///
    /// @param message                  The typed message to serialise.
    /// @param buffer                   Inout: destination buffer; capacity must be
    ///                                 large enough to hold the encoded message.
    /// @param marshalling_interface    Inout: protocol-specific primitive encoder.
    /// @param bytes_consumed           Output: bytes written to @a buffer.
    /// @param return_code              Output: NO_ERROR on success;
    ///                                 DATA_BUFFER_TOO_SMALL if buffer is too small.
    virtual void Serialize(
        const MESSAGE_TYPE&    message,
        DATA_BUFFER_TYPE&      buffer,
        Primitive_Marshalling& marshalling_interface,
        BYTE_SIZE_TYPE&        bytes_consumed,
        RETURN_CODE_TYPE&      return_code) = 0;

    /// Deserialise raw bytes into a typed message.
    ///
    /// @param buffer                   Source buffer containing encoded data.
    /// @param message                  Inout: destination message; GUID must be
    ///                                 set by caller or CALLEE_PROVIDES_GUID.
    /// @param marshalling_interface    Inout: protocol-specific primitive decoder.
    /// @param bytes_consumed           Output: bytes consumed from @a buffer.
    /// @param return_code              Output: NO_ERROR on success.
    virtual void DeSerialize(
        const DATA_BUFFER_TYPE& buffer,
        MESSAGE_TYPE&           message,
        Primitive_Marshalling&  marshalling_interface,
        BYTE_SIZE_TYPE&         bytes_consumed,
        RETURN_CODE_TYPE&       return_code) = 0;
};

//----------------------------------------------------------------------------
// Serialization
//----------------------------------------------------------------------------

/// Factory interface for obtaining per-message-type serialisation helpers.
///
/// The TPM calls Get_Serialization() to retrieve the Message_Serialization
/// implementation for a given message type GUID.
class Serialization {
public:
    virtual ~Serialization() {}

    /// Retrieve the Message_Serialization implementation for a message type.
    ///
    /// @param message_type_id  GUID identifying the message type.
    /// @param serialization    Output: pointer to the Message_Serialization
    ///                         implementation for the given type.
    /// @param return_code      Output: NO_ERROR if the type is known;
    ///                         INVALID_PARAM if the GUID is unrecognised.
    virtual void Get_Serialization(
        GUID_TYPE              message_type_id,
        Message_Serialization*& serialization,
        RETURN_CODE_TYPE&      return_code) = 0;
};

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_SERIALIZATION_HPP
