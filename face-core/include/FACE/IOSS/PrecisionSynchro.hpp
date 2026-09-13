// FACE/IOSS/PrecisionSynchro.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<PrecisionSynchro::ReadWriteBuffer> PrecisionSynchro;
// FACE Technical Standard Edition 3.2
//
// Precision Synchro I/O Service – angular position/velocity using 64-bit
// (int64) high-precision values.

#ifndef FACE_IOSS_PRECISIONSYNCHRO_HPP
#define FACE_IOSS_PRECISIONSYNCHRO_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace PrecisionSynchro {

// -- Service-specific types -------------------------------------------------

/// Specifies how to interpret the angle value in a write.
enum ANGLE_INTENT_TYPE {
    USE_ANGLE,      ///< move to given angle at given velocity
    DISREGARD_ANGLE ///< disregard angle; simply turn at given velocity
};

/// Precision Synchro read/write payload.
struct ReadWriteBuffer {
    ANGLE_INTENT_TYPE angle_intent;
    int64_t           angle;    ///< target angle (implementation-defined units)
    int64_t           velocity; ///< angular velocity
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE MIN_VALUE  = 0; ///< FACE_LONGLONG
static constexpr IO_PARAMETER_ID_TYPE MAX_VALUE  = 1; ///< FACE_LONGLONG
static constexpr IO_PARAMETER_ID_TYPE INIT_VALUE = 2; ///< FACE_LONGLONG
static constexpr IO_PARAMETER_ID_TYPE PRECISION  = 3; ///< FACE_LONGDOUBLE

// Note: no defined bus status types for the PrecisionSynchro I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace PrecisionSynchro
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_PRECISIONSYNCHRO_HPP
