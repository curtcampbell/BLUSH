// FACE/TSS/Serialization_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::Serialization> Serialization_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_SERIALIZATION_INJECTABLE_HPP
#define FACE_TSS_SERIALIZATION_INJECTABLE_HPP

#include "Common.hpp"
#include "Serialization.hpp"

namespace FACE {
namespace TSS {
namespace Serialization_Injectable {

/// Expanded Injectable interface for FACE::TSS::Serialization.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        Serialization*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace Serialization_Injectable
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_SERIALIZATION_INJECTABLE_HPP
