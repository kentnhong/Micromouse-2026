#include "motion.h"

static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.25f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * 3.14159265358979323846f) /
    (kGearRatio * kTicksPerMotorRev);
static constexpr float kPidTargetSpeedMmPerSec = 30.0f;
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
    hw.motor.drive(Drv8231::Direction::FORWARD, 0);
    pid.set_output_limits(-0.25f, 0.25f);
}

bool Motion::update()
{
    float target_ticks_per_sec = 0.0f;

    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);

    pid.set_target_ticks_per_sec(kPidTargetSpeedMmPerSec / kMmPerTick,
                                 target_ticks_per_sec);

    return pid.update(target_ticks_per_sec, Drv8231::Direction::FORWARD,
                      sample_ticks, encoder_timing.sample_time_sec);
}

}  // namespace MM

