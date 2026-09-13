// FACE/TSS/Base_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::Base> Base_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_BASE_INJECTABLE_HPP
#define FACE_TSS_BASE_INJECTABLE_HPP

#include "Common.hpp"
#include "Base.hpp"

namespace FACE {
namespace TSS {
namespace Base_Injectable {

/// Expanded Injectable interface for FACE::TSS::Base.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        Base*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace Base_Injectable
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_BASE_INJECTABLE_HPP
