// FACE/TSS/Primitive_Marshalling.hpp
// C++ mapping of FACE/TSS/Primitive_Marshalling.idl
// FACE Technical Standard Edition 3.2
//
// Protocol-specific base-type serialisation interface.
// A Marshal/Unmarshal pair is provided for each IDL primitive type.

#ifndef FACE_TSS_PRIMITIVE_MARSHALLING_HPP
#define FACE_TSS_PRIMITIVE_MARSHALLING_HPP

#include "Common.hpp"

namespace FACE {
namespace TSS {

/// Protocol-specific base-type serialisation interface.
///
/// Implementors provide wire-format encoding/decoding for each IDL
/// primitive type so that higher-level serialisation code (Message_Serialization)
/// can be composed from these building blocks without depending on a
/// specific wire format.
class Primitive_Marshalling {
public:
    virtual ~Primitive_Marshalling() {}

    // ----- short / int16_t ------------------------------------------------

    virtual void Marshal_short(
        int16_t              data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_short(
        const DATA_BUFFER_TYPE& buffer,
        int16_t&             data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- long / int32_t -------------------------------------------------

    virtual void Marshal_long(
        int32_t              data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_long(
        const DATA_BUFFER_TYPE& buffer,
        int32_t&             data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- long long / int64_t --------------------------------------------

    virtual void Marshal_long_long(
        int64_t              data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_long_long(
        const DATA_BUFFER_TYPE& buffer,
        int64_t&             data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- unsigned short / uint16_t --------------------------------------

    virtual void Marshal_unsigned_short(
        uint16_t             data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_unsigned_short(
        const DATA_BUFFER_TYPE& buffer,
        uint16_t&            data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- unsigned long / uint32_t ---------------------------------------

    virtual void Marshal_unsigned_long(
        uint32_t             data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_unsigned_long(
        const DATA_BUFFER_TYPE& buffer,
        uint32_t&            data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- unsigned long long / uint64_t ----------------------------------

    virtual void Marshal_unsigned_long_long(
        uint64_t             data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_unsigned_long_long(
        const DATA_BUFFER_TYPE& buffer,
        uint64_t&            data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- float ----------------------------------------------------------

    virtual void Marshal_float(
        float                data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_float(
        const DATA_BUFFER_TYPE& buffer,
        float&               data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- double ---------------------------------------------------------

    virtual void Marshal_double(
        double               data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_double(
        const DATA_BUFFER_TYPE& buffer,
        double&              data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- long double ----------------------------------------------------

    virtual void Marshal_long_double(
        long double          data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_long_double(
        const DATA_BUFFER_TYPE& buffer,
        long double&         data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- char -----------------------------------------------------------

    virtual void Marshal_char(
        char                 data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_char(
        const DATA_BUFFER_TYPE& buffer,
        char&                data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- boolean --------------------------------------------------------

    virtual void Marshal_boolean(
        bool                 data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_boolean(
        const DATA_BUFFER_TYPE& buffer,
        bool&                data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- octet / uint8_t ------------------------------------------------

    virtual void Marshal_octet(
        uint8_t              data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    virtual void Unmarshal_octet(
        const DATA_BUFFER_TYPE& buffer,
        uint8_t&             data,
        BYTE_SIZE_TYPE&      bytes_consumed,
        RETURN_CODE_TYPE&    return_code) = 0;

    // ----- string ---------------------------------------------------------

    virtual void Marshal_string(
        UNBOUNDED_STRING_TYPE   data,
        const DATA_BUFFER_TYPE& buffer,
        BYTE_SIZE_TYPE&         bytes_consumed,
        RETURN_CODE_TYPE&       return_code) = 0;

    virtual void Unmarshal_string(
        const DATA_BUFFER_TYPE& buffer,
        UNBOUNDED_STRING_TYPE&  data,
        BYTE_SIZE_TYPE&         bytes_consumed,
        RETURN_CODE_TYPE&       return_code) = 0;
};

} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_PRIMITIVE_MARSHALLING_HPP
