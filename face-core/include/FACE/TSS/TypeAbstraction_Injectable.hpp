// FACE/TSS/TypeAbstraction_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::TypeAbstraction::TypeAbstractionTS> TypeAbstractionTS_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_TYPEABSTRACTION_INJECTABLE_HPP
#define FACE_TSS_TYPEABSTRACTION_INJECTABLE_HPP

#include "Common.hpp"
#include "TypeAbstraction.hpp"

namespace FACE {
namespace TSS {
namespace TypeAbstraction {
namespace TypeAbstractionTS_Injectable {

/// Expanded Injectable interface for FACE::TSS::TypeAbstraction::TypeAbstractionTS.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        TypeAbstractionTS*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace TypeAbstractionTS_Injectable
} // namespace TypeAbstraction
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_TYPEABSTRACTION_INJECTABLE_HPP
