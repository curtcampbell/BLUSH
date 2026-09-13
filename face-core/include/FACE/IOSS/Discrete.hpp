// FACE/IOSS/Discrete.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<Discrete::ReadWriteBuffer> Discrete;
// FACE Technical Standard Edition 3.2
//
// Discrete I/O Service – single-bit digital I/O (discrete inputs/outputs).

#ifndef FACE_IOSS_DISCRETE_HPP
#define FACE_IOSS_DISCRETE_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace Discrete {

// -- Service-specific types -------------------------------------------------

/// State of a discrete channel.
typedef uint16_t DISCRETE_STATE_TYPE;

static constexpr DISCRETE_STATE_TYPE LOW          = 0; ///< logic low
static constexpr DISCRETE_STATE_TYPE HIGH         = 1; ///< logic high
static constexpr DISCRETE_STATE_TYPE OPEN         = 2; ///< open circuit
static constexpr DISCRETE_STATE_TYPE UNDETERMINED = 3; ///< state unknown

/// Discrete read/write payload: state of one discrete channel.
struct ReadWriteBuffer {
    DISCRETE_STATE_TYPE state;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE MAX_INPUTS           = 0; ///< FACE_LONGLONG: max discrete inputs
static constexpr IO_PARAMETER_ID_TYPE MAX_OUTPUTS          = 1; ///< FACE_LONGLONG: max discrete outputs
static constexpr IO_PARAMETER_ID_TYPE DIRECTION            = 2; ///< FACE_BOOLEAN: FALSE=in, TRUE=out
static constexpr IO_PARAMETER_ID_TYPE INITIAL_OUTPUT_VALUE = 3; ///< FACE_BOOLEAN: initial output state

// Note: no defined bus status types for the Discrete I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace Discrete
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_DISCRETE_HPP
