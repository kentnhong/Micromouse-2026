#include "motion.h"

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

// Nominal speed target for closed-loop velocity control.
static constexpr float kPidTargetSpeedMmPerSec = 1000.0f;
static constexpr float kProfileMinSpeedMmPerSec = 0.0f;
static constexpr float kProfileAccelMmPerSec2 = 2500.0f;
static constexpr float kProfileDecelMmPerSec2 = 2500.0f;
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
    const float target_ticks_per_sec = kPidTargetSpeedMmPerSec / kMmPerTick;
    const float min_ticks_per_sec = kProfileMinSpeedMmPerSec / kMmPerTick;
    const float accel_ticks_per_sec2 = kProfileAccelMmPerSec2 / kMmPerTick;
    const float decel_ticks_per_sec2 = kProfileDecelMmPerSec2 / kMmPerTick;

    // Distance goal = 0 means ramp-to-cruise behavior and hold max speed.
    profile.configure(min_ticks_per_sec, target_ticks_per_sec,
                      accel_ticks_per_sec2, decel_ticks_per_sec2, 0);

    // Keep command authority conservative while tuning gains.
    pid.set_output_limits(-0.25f, 0.25f);
}

bool Motion::update()
{
    // Sample over a fixed window to get deterministic tick deltas per update.
    const uint32_t start_cycles = hw.encoder.get_time_cycles();
    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);

    // Compute dt from the same cycle counter used by sampling so PID timing
    // matches actual elapsed time.
    const uint32_t end_cycles = hw.encoder.get_time_cycles();
    const uint32_t cycles_per_us = hw.encoder.cycles_per_us();

    float dt_sec = encoder_timing.sample_time_sec;
    if (cycles_per_us != 0)
    {
        const uint32_t delta_cycles = end_cycles - start_cycles;
        const float dt_us = static_cast<float>(delta_cycles) /
                            static_cast<float>(cycles_per_us);
        dt_sec = dt_us / 1'000'000.0f;
    }

    if (dt_sec <= 0.0f)
    {
        hw.motor.drive(Drv8231::Direction::COAST, 0);
        return false;
    }

    // Advance the speed profile using measured encoder progress.
    const EncoderInput progress{.left_ticks = sample_ticks,
                                .right_ticks = sample_ticks};
    profile.trapezoidal(progress, dt_sec);

    float target_ticks_per_sec = 0.0f;
    pid.set_target_ticks_per_sec(profile.get_speed_setpoint(),
                                 target_ticks_per_sec);

    // Closed-loop control: PID computes motor drive from setpoint vs measured
    // encoder ticks/dt.
    return pid.update(target_ticks_per_sec, Drv8231::Direction::FORWARD,
                      sample_ticks, dt_sec);
}

}  // namespace MM
