#include "motion.h"

#include <cstdint>

static constexpr float kPi = 3.14159265358979323846f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

static constexpr float kPidTargetSpeedMmPerSec =
    190.0f;  // TODO: fix this value so it's in one cell
static constexpr float kTargetTicksPerSec =
    kPidTargetSpeedMmPerSec / kMmPerTick;

static constexpr uint32_t kFallbackEncoderSampleUs = 1'000;

namespace MM
{
Motion::Motion(Board& hw, const Gains& gains)
    : hw(hw),
      pid(hw.motor_left, gains),
      encoder_timing(Sample::init_encoder_timing(
          hw.encoder_left, hw.encoder_sample_us != 0
                               ? hw.encoder_sample_us
                               : kFallbackEncoderSampleUs))
{
    pid.set_output_limits(-0.60f, 0.60f);
}

bool Motion::update()
{
    const int32_t sample_ticks =
        Sample::sample_encoder(hw.encoder_left, encoder_timing);

    pid.update(kTargetTicksPerSec, Drv8231::Direction::FORWARD, sample_ticks,
               encoder_timing.sample_time_sec);

    const PidDebug& pd = pid.get_debug();
    debug_data.sample_ticks = sample_ticks;
    debug_data.error = pd.error;
    debug_data.p_term = pd.p;
    debug_data.i_term = pd.i_term;
    debug_data.d_term = pd.d;
    debug_data.output = pd.output;

    return true;
}

}  // namespace MM
