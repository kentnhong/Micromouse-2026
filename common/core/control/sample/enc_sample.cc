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

int32_t Sample::sample_encoder(Encoder& encoder, const EncoderTiming& timing)
{
    const uint32_t start_time = encoder.get_time_cycles();
    while ((encoder.get_time_cycles() - start_time) < timing.sample_cycles)
    {
    }

    const int32_t ticks = encoder.get_ticks();
    encoder.reset_ticks();

    return ticks;
}

MM::EncoderInput Sample::sample_encoders(Encoder& phase_a, Encoder& phase_b,
                                         const EncoderTiming& timing)
{
    const uint32_t start_time = phase_a.get_time_cycles();

    while ((phase_a.get_time_cycles() - start_time) < timing.sample_cycles)
    {
    }

    while ((phase_b.get_time_cycles() - start_time) < timing.sample_cycles)
    {
    }

    EncoderInput encoder{.left_ticks = phase_a.get_ticks(),
                         .right_ticks = phase_b.get_ticks()};

    phase_a.reset_ticks();
    phase_b.reset_ticks();

    return encoder;
}

}  // namespace MM
