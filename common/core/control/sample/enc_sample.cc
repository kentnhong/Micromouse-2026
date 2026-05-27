#include "enc_sample.h"

namespace MM
{

static constexpr float kUsToSec = 1'000'000.0f;

Sample::EncoderTiming Sample::init_encoder_timing(Encoder& timing_encoder,
                                                  uint32_t sample_time_us)
{
    timing_encoder.init_cycle_counter();

    uint32_t cycles_per_us = timing_encoder.cycles_per_us();

    return {.sample_cycles = sample_time_us * cycles_per_us,
            .sample_time_sec = static_cast<float>(sample_time_us) / kUsToSec};
}

float Sample::sample_encoder(Encoder& encoder, const EncoderTiming& timing)
{
    // Two-phase T method: sync to a tick edge first, then measure the next full
    // tick-to-tick interval. Eliminates phase-ambiguity spikes from a single-phase
    // implementation (which can measure a fraction of an interval and report a
    // falsely high velocity).
    //
    // timing.sample_cycles is the total timeout budget shared across both phases.
    const uint32_t deadline = encoder.get_time_cycles() + timing.sample_cycles;
    const float cpu_freq =
        static_cast<float>(timing.sample_cycles) / timing.sample_time_sec;

    // Phase 1: wait for the next tick edge (sync)
    const int32_t pre_ticks = encoder.get_ticks();
    int32_t sync_ticks;
    uint32_t t1;
    do
    {
        sync_ticks = encoder.get_ticks();
        t1 = encoder.get_time_cycles();
        if (t1 >= deadline)
            return 0.0f;
    } while (sync_ticks == pre_ticks);

    // Phase 2: measure time to the following tick edge
    int32_t next_ticks;
    uint32_t t2;
    do
    {
        next_ticks = encoder.get_ticks();
        t2 = encoder.get_time_cycles();
        if (t2 >= deadline)
            return 0.0f;
    } while (next_ticks == sync_ticks);

    const float dt_sec = static_cast<float>(t2 - t1) / cpu_freq;
    return static_cast<float>(next_ticks - sync_ticks) / dt_sec;
}

MM::EncoderInput Sample::sample_encoders(Encoder& phase_a, Encoder& phase_b,
                                         const EncoderTiming& timing)
{
    const int32_t start_time = phase_a.get_time_cycles();

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