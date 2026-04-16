#include "enc_sample.h"

namespace MM
{

static constexpr float kUsToSec = 1'000'000.0f;

Sample::EncoderTiming Sample::init_encoder_timing(Encoder& timing_encoder,
                                                  uint32_t sample_time_us)
{
    timing_encoder.init_cycle_counter();

    const uint32_t cycles_per_us = timing_encoder.cycles_per_us();

    return {.sample_cycles = sample_time_us * cycles_per_us,
            .sample_time_sec = static_cast<float>(sample_time_us) / kUsToSec};
}

MM::EncoderInput Sample::sample_encoders(Encoder& left_encoder,
                                         Encoder& right_encoder,
                                         const EncoderTiming& timing)
{
    const uint32_t start_time = left_encoder.get_time_cycles();
    while ((left_encoder.get_time_cycles() - start_time) < timing.sample_cycles)
    {
    }

    EncoderInput encoder{.left_ticks = left_encoder.get_ticks(),
                         .right_ticks = right_encoder.get_ticks()};

    left_encoder.reset_ticks();
    right_encoder.reset_ticks();

    return encoder;
}

}  // namespace MM
