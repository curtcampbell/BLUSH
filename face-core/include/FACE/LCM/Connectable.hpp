// FACE/LCM/Connectable.hpp
// C++ mapping of FACE/LCM/Connectable.idl
// FACE Technical Standard Edition 3.2
//
// LCM Connectable capability.
// Notified by the Component Framework when the instance is being
// connected or disconnected.

#ifndef FACE_LCM_CONNECTABLE_HPP
#define FACE_LCM_CONNECTABLE_HPP

#include "../Common.hpp"

namespace FACE {
namespace LCM {
namespace Connectable {

/// LCM Connectable instance interface.
///
/// A UoC that needs notification of framework connect/disconnect events
/// implements this interface.
///
/// NOTE: FACE does not define Create/Destroy interfaces; those are
///       addressed by language bindings.  The LCM interfaces assume
///       an already-instantiated software object.
class ConnectableInstance {
public:
    virtual ~ConnectableInstance() {}

    /// Called by the Component Framework when the instance is being connected.
    ///
    /// Invoked before or after the actual connection (framework's choice),
    /// giving the instance an opportunity to perform appropriate behaviour.
    ///
    /// @param configuration  Resource locator for connection configuration.
    /// @param return_code    Output: NO_ERROR on success.
    virtual void Framework_Connect(
        const CONFIGURATION_RESOURCE& configuration,
        RETURN_CODE_TYPE&             return_code) = 0;

    /// Called by the Component Framework when the instance is being disconnected.
    ///
    /// Invoked before or after the actual disconnection (framework's choice).
    ///
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Framework_Disconnect(
        RETURN_CODE_TYPE& return_code) = 0;
};

} // namespace Connectable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_CONNECTABLE_HPP
