// FACE/IOSS/Synchro.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<Synchro::ReadWriteBuffer> Synchro;
// FACE Technical Standard Edition 3.2
//
// Synchro I/O Service – angular position/velocity using 32-bit (int32) precision.

#ifndef FACE_IOSS_SYNCHRO_HPP
#define FACE_IOSS_SYNCHRO_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace Synchro {

// -- Service-specific types -------------------------------------------------

/// Specifies how to interpret the angle value in a write.
enum ANGLE_INTENT_TYPE {
    USE_ANGLE,      ///< move to given angle at given velocity
    DISREGARD_ANGLE ///< disregard angle; simply turn at given velocity
};

/// Synchro read/write payload.
struct ReadWriteBuffer {
    ANGLE_INTENT_TYPE angle_intent;
    int32_t           angle;    ///< target angle (implementation-defined units)
    int32_t           velocity; ///< angular velocity
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE MIN_VALUE  = 0; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE MAX_VALUE  = 1; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE INIT_VALUE = 2; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE PRECISION  = 3; ///< FACE_LONGDOUBLE

// Note: no defined bus status types for the Synchro I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace Synchro
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_SYNCHRO_HPP
