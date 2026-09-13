// FACE/IOSS/MultiChannelAnalog.hpp
// C++ binding of the expanded IDL:
//   module IO_Service_Module<MultiChannelAnalog::ReadWriteBuffer> MultiChannelAnalog;
// FACE Technical Standard Edition 3.2
//
// Multi-Channel Analog I/O Service – up to 32 ADC/DAC channels per connection.

#ifndef FACE_IOSS_MULTICHANNELANALOG_HPP
#define FACE_IOSS_MULTICHANNELANALOG_HPP

#include "IOS.hpp"

namespace FACE {
namespace IOSS {
namespace MultiChannelAnalog {

// -- Service-specific types -------------------------------------------------

/// Maximum number of analog channels per connection.
static constexpr uint16_t MAX_CHANNELS_PER_CONNECTION = 32;

typedef int32_t ANALOG_VALUE_TYPE;

/// Bounded array of analog channel values
/// (IDL: sequence<ANALOG_VALUE_TYPE, MAX_CHANNELS_PER_CONNECTION>).
struct MULTICHANNEL_BUFFER_TYPE {
    ANALOG_VALUE_TYPE data[MAX_CHANNELS_PER_CONNECTION];
    uint16_t          length; ///< number of valid channels

    MULTICHANNEL_BUFFER_TYPE() : length(0) {}
};

/// Multi-channel analog read/write payload.
struct ReadWriteBuffer {
    MULTICHANNEL_BUFFER_TYPE analog_channels;
};

// -- Configuration parameter IDs -------------------------------------------

static constexpr IO_PARAMETER_ID_TYPE MIN_VALUE   = 0; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE MAX_VALUE   = 1; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE INIT_VALUE  = 2; ///< FACE_LONG
static constexpr IO_PARAMETER_ID_TYPE PRECISION   = 3; ///< FACE_DOUBLE
static constexpr IO_PARAMETER_ID_TYPE DIRECTION   = 4; ///< FACE_BOOLEAN: FALSE=in, TRUE=out
static constexpr IO_PARAMETER_ID_TYPE GAIN        = 5; ///< FACE_LONGDOUBLE: voltage gain
static constexpr IO_PARAMETER_ID_TYPE OFFSET      = 6; ///< FACE_LONGDOUBLE: voltage offset
static constexpr IO_PARAMETER_ID_TYPE NUM_INPUTS  = 7; ///< FACE_UNSIGNEDSHORT (<= MAX_CHANNELS)
static constexpr IO_PARAMETER_ID_TYPE NUM_OUTPUTS = 8; ///< FACE_UNSIGNEDSHORT (<= MAX_CHANNELS)

// Note: no defined bus status types for the MultiChannelAnalog I/O Service.

// -- IO_Service_Module expansion -------------------------------------------

typedef ReadWriteBuffer PAYLOAD_DATA_MSG_TYPE;
#include "detail/IO_Service_Expansion.inc"

} // namespace MultiChannelAnalog
} // namespace IOSS
} // namespace FACE

#endif // FACE_IOSS_MULTICHANNELANALOG_HPP
