#include "motion.h"

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

// TEMP: open-loop top-speed test (so PID setpoint should not matter)
static constexpr float kPidTargetSpeedMmPerSec = 1000.0f;
static constexpr uint32_t kFallbackEncoderSampleUs = 1'000;

namespace MM
{
Motion::Motion(Board& hw, const Gains& gains)
    : hw(hw),
      pid(hw.motor, gains),
      encoder_timing(Sample::init_encoder_timing(
          hw.encoder, hw.encoder_sample_us != 0 ? hw.encoder_sample_us
                                                : kFallbackEncoderSampleUs))
{
    hw.motor.drive(Drv8231::Direction::REVERSE, 50);
    pid.set_output_limits(-0.25f, 0.25f);
}

bool Motion::update()
{
    float target_ticks_per_sec = 0.0f;

    const uint32_t start_cycles = hw.encoder.get_time_cycles();
    (void)start_cycles;

    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);
    (void)sample_ticks;

    // Compute dt from the same DWT cycle counter actually used for the
    // busy-wait.
    const uint32_t end_cycles = hw.encoder.get_time_cycles();
    const uint32_t cycles_per_us = hw.encoder.cycles_per_us();

    float dt_sec = 0.0f;
    if (cycles_per_us != 0)
    {
        // Note: end_cycles - start_cycles is safe if the debug loop
        // observes dt within a single DWT wrap period.
        const uint32_t delta_cycles = end_cycles - start_cycles;
        const float dt_us = static_cast<float>(delta_cycles) /
                            static_cast<float>(cycles_per_us);
        dt_sec = dt_us / 1'000'000.0f;
    }

    pid.set_target_ticks_per_sec(kPidTargetSpeedMmPerSec / kMmPerTick,
                                 target_ticks_per_sec);

    // TOP-SPEED TEST MODE: drive motor open-loop and do not run PID.
    // Still read sample_ticks so debugger shows encoder delta.
    hw.motor.drive(Drv8231::Direction::FORWARD, 80);

    (void)pid;
    (void)target_ticks_per_sec;
    (void)dt_sec;

    return true;
}

}  // namespace MM
