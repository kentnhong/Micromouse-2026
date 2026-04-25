#include "motion.h"

static constexpr float kTargetDistanceMm = 180.0f;
static constexpr float kMmPerMeter = 1000.0f;
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
    hw.left_motor.set_direction(Drv8231::Direction::FORWARD);
    hw.right_motor.set_direction(Drv8231::Direction::FORWARD);
}

bool Motion::update()
{
    PID::Target target{};

    // Get the encoder timing configuration based on the desired sample time
    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.left_encoder, hw.encoder_sample_us);

    EncoderInput sample_encoder = Sample::sample_encoders(
        hw.left_encoder, hw.right_encoder, encoder_timing);

    total_encoder.left_ticks += sample_encoder.left_ticks;
    total_encoder.right_ticks += sample_encoder.right_ticks;

    const Trapezoidal::VelocitySetpoint setpoint = profile.trapezoidal(
        kTargetDistanceMm, total_encoder, kTicksToMm, kStraight);

    // Convert the velocity setpoints from mm/s to m/s for the PID controller
    pid.set_target_speed(setpoint.left / kMmPerMeter,
                         setpoint.right / kMmPerMeter, target);

    return profile.is_complete();
}

}  // namespace MM