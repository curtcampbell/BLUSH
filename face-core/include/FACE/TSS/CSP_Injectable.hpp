// FACE/TSS/CSP_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::CSP::CSP> CSP_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_CSP_INJECTABLE_HPP
#define FACE_TSS_CSP_INJECTABLE_HPP

#include "Common.hpp"
#include "CSP.hpp"

namespace FACE {
namespace TSS {
namespace CSP {
namespace CSP_Injectable {

/// Expanded Injectable interface for FACE::TSS::CSP::CSP.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        CSP*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace CSP_Injectable
} // namespace CSP
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_CSP_INJECTABLE_HPP
