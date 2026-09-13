// FACE/LCM/Configurable_Injectable.hpp
// C++ binding of the expanded IDL:
//   module Injectable<FACE::LCM::Configurable::ConfigurableInstance> ConfigurableInstance_Injectable;
// FACE Technical Standard Edition 3.2

#ifndef FACE_LCM_CONFIGURABLE_INJECTABLE_HPP
#define FACE_LCM_CONFIGURABLE_INJECTABLE_HPP

#include "../Common.hpp"
#include "Configurable.hpp"

namespace FACE {
namespace LCM {
namespace Configurable {
namespace ConfigurableInstance_Injectable {

/// Expanded Injectable interface for FACE::LCM::Configurable::ConfigurableInstance.
class Injectable {
public:
    virtual ~Injectable() {}

    virtual void Set_Reference(
        const STRING_TYPE& interface_name,
        ConfigurableInstance*&          interface_reference,
        GUID_TYPE          id,
        RETURN_CODE_TYPE&  return_code) = 0;
};

} // namespace ConfigurableInstance_Injectable
} // namespace Configurable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_CONFIGURABLE_INJECTABLE_HPP
