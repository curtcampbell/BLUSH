// FACE/LCM/Configurable.hpp
// C++ mapping of FACE/LCM/Configurable.idl
// FACE Technical Standard Edition 3.2
//
// Life-Cycle Management (LCM) Configurable capability.
// Called by the Component Framework to provide configuration parameters
// at the appropriate execution point in a component's life-cycle.

#ifndef FACE_LCM_CONFIGURABLE_HPP
#define FACE_LCM_CONFIGURABLE_HPP

#include "../Common.hpp"

namespace FACE {
namespace LCM {
namespace Configurable {

/// LCM Configurable instance interface.
///
/// A UoC that supports runtime configuration implements this interface.
/// The Component Framework invokes Configure() at the appropriate
/// life-cycle point with a configuration resource locator.
///
/// NOTE: FACE does not define Create/Destroy interfaces; those are
///       addressed by language bindings.  The LCM interfaces assume
///       an already-instantiated software object.
class ConfigurableInstance {
public:
    virtual ~ConfigurableInstance() {}

    /// Provide configuration parameters to the instance.
    ///
    /// @param configuration  Locator (file name, URI, etc.) for the
    ///                       configuration resource.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Configure(
        const CONFIGURATION_RESOURCE& configuration,
        RETURN_CODE_TYPE&             return_code) = 0;
};

} // namespace Configurable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_CONFIGURABLE_HPP
