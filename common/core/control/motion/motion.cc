#include "motion.h"

static constexpr float kTargetDistanceMm = 180.0f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.25f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kTicksToMm =
    (kWheelDiameterMm * std::numbers::pi_v<float>) /
    (kGearRatio * kTicksPerMotorRev);
static constexpr uint8_t kStraight = 0;

namespace MM
{
Motion::Motion(Board& hw)
    : hw(hw), pid(PID::PIDConfig{}), profile(), total_encoder{}
{
    // Direction setup
    hw.motor.set_direction(Drv8231::Direction::FORWARD);
}

bool Motion::update()
{
    float target_ticks_per_sec = 0.0f;

    // Get the encoder timing configuration based on the desired sample time
    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.encoder, hw.encoder_sample_us);

    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);

    total_encoder.left_ticks += sample_ticks;
    total_encoder.right_ticks += sample_ticks;

    const Trapezoidal::VelocitySetpoint setpoint = profile.trapezoidal(
        kTargetDistanceMm, total_encoder, kTicksToMm, kStraight);

    // Convert the velocity setpoint from mm/s to ticks/s for the generic PID.
    pid.set_target_ticks_per_sec(setpoint.left / kTicksToMm,
                                 target_ticks_per_sec);

    return profile.is_complete();
}

}  // namespace MM
