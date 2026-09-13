// FACE/IOSS/MultiChannelDiscrete.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<MultiChannelDiscrete::ReadWriteBuffer> MultiChannelDiscrete;
// FACE Technical Standard Edition 3.2
//
// Multi-Channel Discrete I/O Service – up to 32 discrete bits packed into
// a single unsigned long (one bit per channel).

#ifndef FACE_IOSS_MULTICHANNELDISCRETE_HPP
#define FACE_IOSS_MULTICHANNELDISCRETE_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace MultiChannelDiscrete {

// -- Service-specific types -------------------------------------------------

/// Number of bits in MULTICHANNEL_BUFFER_TYPE; each bit represents one channel.
static constexpr uint16_t MAX_CHANNELS_PER_CONNECTION = 32;

/// Bitmask of up to 32 discrete channel states (bit N = channel N).
typedef uint32_t MULTICHANNEL_BUFFER_TYPE;

/// Multi-channel discrete read/write payload.
struct ReadWriteBuffer {
    MULTICHANNEL_BUFFER_TYPE discrete_channels;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE NUM_INPUTS  = 0; ///< FACE_UNSIGNEDSHORT (<= MAX_CHANNELS)
static constexpr IO_PARAMETER_ID_TYPE NUM_OUTPUTS = 1; ///< FACE_UNSIGNEDSHORT (<= MAX_CHANNELS)

// Note: no defined bus status types for the MultiChannelDiscrete I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace MultiChannelDiscrete
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_MULTICHANNELDISCRETE_HPP
