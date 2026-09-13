// FACE/LCM/Initializable.hpp
// C++ mapping of FACE/LCM/Initializable.idl
// FACE Technical Standard Edition 3.2
//
// LCM Initializable capability.
// Provides Initialize and Finalize hooks at appropriate life-cycle points.

#ifndef FACE_LCM_INITIALIZABLE_HPP
#define FACE_LCM_INITIALIZABLE_HPP

#include "../Common.hpp"

namespace FACE {
namespace LCM {
namespace Initializable {

/// LCM Initializable instance interface.
///
/// A UoC that requires explicit initialisation and finalisation calls
/// from the Component Framework implements this interface.
///
/// NOTE: FACE does not define Create/Destroy interfaces; those are
///       addressed by language bindings.  The LCM interfaces assume
///       an already-instantiated software object.
class InitializableInstance {
public:
    virtual ~InitializableInstance() {}

    /// Perform initialisation at the corresponding life-cycle point.
    ///
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Initialize(
        RETURN_CODE_TYPE& return_code) = 0;

    /// Perform finalisation at the corresponding life-cycle point.
    ///
    /// @param return_code  Output: NO_ERROR on success.
    virtual void Finalize(
        RETURN_CODE_TYPE& return_code) = 0;
};

} // namespace Initializable
} // namespace LCM
} // namespace FACE

#endif // FACE_LCM_INITIALIZABLE_HPP
