// FACE/TSS/Primitive_Marshalling_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::Primitive_Marshalling> Primitive_Marshalling_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_PRIMITIVE_MARSHALLING_INJECTABLE_HPP
#define FACE_TSS_PRIMITIVE_MARSHALLING_INJECTABLE_HPP

#include "Common.hpp"
#include "Primitive_Marshalling.hpp"

namespace FACE {
namespace TSS {
namespace Primitive_Marshalling_Injectable {

/// Expanded Injectable interface for FACE::TSS::Primitive_Marshalling.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        Primitive_Marshalling*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace Primitive_Marshalling_Injectable
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_PRIMITIVE_MARSHALLING_INJECTABLE_HPP
