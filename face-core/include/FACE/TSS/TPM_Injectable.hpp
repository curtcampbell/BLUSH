// FACE/TSS/TPM_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::TSS::TPM::TPMTS> TPMTS_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_TSS_TPM_INJECTABLE_HPP
#define FACE_TSS_TPM_INJECTABLE_HPP

#include "Common.hpp"
#include "TPM.hpp"

namespace FACE {
namespace TSS {
namespace TPM {
namespace TPMTS_Injectable {

/// Expanded Injectable interface for FACE::TSS::TPM::TPMTS.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        TPMTS*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace TPMTS_Injectable
} // namespace TPM
} // namespace TSS
} // namespace FACE

#endif // FACE_TSS_TPM_INJECTABLE_HPP
