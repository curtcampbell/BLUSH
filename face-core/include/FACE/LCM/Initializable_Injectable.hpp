// FACE/LCM/Initializable_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::LCM::Initializable::InitializableInstance> InitializableInstance_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_LCM_INITIALIZABLE_INJECTABLE_HPP
#define FACE_LCM_INITIALIZABLE_INJECTABLE_HPP

#include "../Common.hpp"
#include "Initializable.hpp"

namespace FACE {
namespace LCM {
namespace Initializable {
namespace InitializableInstance_Injectable {

/// Expanded Injectable interface for FACE::LCM::Initializable::InitializableInstance.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        InitializableInstance*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace InitializableInstance_Injectable
} // namespace Initializable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_INITIALIZABLE_INJECTABLE_HPP
