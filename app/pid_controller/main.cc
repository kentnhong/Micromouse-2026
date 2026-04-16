/**
* @file main.cc
* @brief PID controller main application
* @author Bex Saw
* @date 3/31/2026
*/

#include <numbers>
#include <tuple>
#include "board.h"
#include "enc_sample.h"
#include "pid.h"
#include "trapezoidal.h"

using namespace MM;

static constexpr float kTargetDistanceMm = 180.0f;
static constexpr float kMmPerMeter = 1000.0f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.25f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kTicksToMm =
    (kWheelDiameterMm * std::numbers::pi_v<float>) /
    (kGearRatio * kTicksPerMotorRev);
static constexpr uint8_t kStraight = 0;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    auto [pid, output, target] = get_pid_bundle();
    Trapezoidal profile;
    EncoderInput total_encoder{};

    uint8_t left_pwm = 0;
    uint8_t right_pwm = 0;

    // Get the encoder timing configuration based on the desired sample time
    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.left_encoder, hw.encoder_sample_us);

    // Encoder Reset
    hw.left_encoder.reset_ticks();
    hw.right_encoder.reset_ticks();

    // Direction setup
    hw.left_motor.set_direction(Drv8231::Direction::FORWARD);
    hw.right_motor.set_direction(Drv8231::Direction::FORWARD);

    while (1)
    {
        EncoderInput sample_encoder = Sample::sample_encoders(
            hw.left_encoder, hw.right_encoder, encoder_timing);

        total_encoder.left_ticks += sample_encoder.left_ticks;
        total_encoder.right_ticks += sample_encoder.right_ticks;

        const Trapezoidal::VelocitySetpoint setpoint = profile.trapezoidal(
            kTargetDistanceMm, total_encoder, kTicksToMm, kStraight);

        // Convert the velocity setpoints from mm/s to m/s for the PID controller
        target.left_speed = setpoint.left / kMmPerMeter;
        target.right_speed = setpoint.right / kMmPerMeter;

        // Update the PID controller with the encoder reading and target velocity
        if (!pid.update(sample_encoder, target, encoder_timing.sample_time_sec,
                        output))
        {
            hw.left_motor.set_speed(0);
            hw.right_motor.set_speed(0);
            continue;
        }

        // Convert the PID output to PWM duty cycle values for the left and right motors
        pid.output_to_pwm_duty_cycle(output, left_pwm, right_pwm);

        // Set the motor speeds based on the calculated PWM duty cycles
        hw.left_motor.set_speed(left_pwm);
        hw.right_motor.set_speed(right_pwm);
    }

    return 0;
}
