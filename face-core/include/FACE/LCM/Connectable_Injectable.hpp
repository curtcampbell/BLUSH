// FACE/LCM/Connectable_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::LCM::Connectable::ConnectableInstance> ConnectableInstance_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_LCM_CONNECTABLE_INJECTABLE_HPP
#define FACE_LCM_CONNECTABLE_INJECTABLE_HPP

#include "../Common.hpp"
#include "Connectable.hpp"

namespace FACE {
namespace LCM {
namespace Connectable {
namespace ConnectableInstance_Injectable {

/// Expanded Injectable interface for FACE::LCM::Connectable::ConnectableInstance.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        ConnectableInstance*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace ConnectableInstance_Injectable
} // namespace Connectable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_CONNECTABLE_INJECTABLE_HPP
