#include "motion.h"

static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.25f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * std::numbers::pi_v<float>) /
    (kGearRatio * kTicksPerMotorRev);
static constexpr float kPidTargetSpeedMmPerSec = 100.0f;

namespace MM
{
Motion::Motion(Board& hw, const Gains& gains) : hw(hw), pid(hw.motor, gains)
{
    hw.motor.drive(Drv8231::Direction::FORWARD, 0);
}

// Motion layer: physical robot units, mm/s
// PID layer: encoder/control units, ticks/s

/// SEQUENCE: target mm/s -> target ticks/s
///           sample encoder ticks
///           PID compares target ticks/s vs measured ticks/sample_time
///           PID outputs PWM

bool Motion::update()
{
    float target_ticks_per_sec = 0.0f;

    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.encoder, hw.encoder_sample_us);

    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder, encoder_timing);

    pid.set_target_ticks_per_sec(kPidTargetSpeedMmPerSec / kMmPerTick,
                                 target_ticks_per_sec);

    return pid.update(target_ticks_per_sec, Drv8231::Direction::FORWARD,
                      sample_ticks, encoder_timing.sample_time_sec);
}

}  // namespace MM
