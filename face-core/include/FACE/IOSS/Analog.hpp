// FACE/IOSS/Analog.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<Analog::ReadWriteBuffer> Analog;
// FACE Technical Standard Edition 3.2
//
// Analog I/O Service – single-channel analog I/O (ADC/DAC).

#ifndef FACE_IOSS_ANALOG_HPP
#define FACE_IOSS_ANALOG_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace Analog {

// -- Service-specific types -------------------------------------------------

/// Analog read/write payload: a single 32-bit integer sample.
struct ReadWriteBuffer {
    int32_t data;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE MIN_VALUE = 0; ///< FACE_LONG: minimum value
static constexpr IO_PARAMETER_ID_TYPE MAX_VALUE = 1; ///< FACE_LONG: maximum value
static constexpr IO_PARAMETER_ID_TYPE INIT_VALUE = 2; ///< FACE_LONG: initial value
static constexpr IO_PARAMETER_ID_TYPE PRECISION  = 3; ///< FACE_DOUBLE: buffer precision
static constexpr IO_PARAMETER_ID_TYPE DIRECTION  = 4; ///< FACE_BOOLEAN: FALSE=in, TRUE=out
static constexpr IO_PARAMETER_ID_TYPE GAIN       = 5; ///< FACE_LONGDOUBLE: voltage gain
static constexpr IO_PARAMETER_ID_TYPE OFFSET     = 6; ///< FACE_LONGDOUBLE: voltage offset

// Note: no defined bus status types for the Analog I/O Service.

// -- IO_Service_Module expansion --------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace Analog
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_ANALOG_HPP
