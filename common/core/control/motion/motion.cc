#include "motion.h"

#include <cstdint>

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

    
static constexpr float kPidTargetSpeedMmPerSec = 190.0f; // 1 cell per second
static constexpr float kDistanceGoalMm = 190.0f; // 1 cell distance

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
    // Keep command authority conservative while tuning gains.
    pid.set_output_limits(-0.60f, 0.60f);
}

bool Motion::update()
{
    // TEST MODE: fixed forward drive — watch sample_ticks in debugger
    volatile int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);
    (void)sample_ticks;

    hw.motor.drive(Drv8231::Direction::FORWARD, 25);
    return true;
}

}  // namespace MM