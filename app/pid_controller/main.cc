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
#include "motion.h"
#include "pid.h"
// #include "trapezoidal.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();
    Motion motion(hw);

    auto [pid, output, target] = get_pid_bundle();

    /// Trapezoidal profile;

    uint8_t left_pwm = 0;
    uint8_t right_pwm = 0;

    // Change here for diff target speeds velocity
    // Testing range from 0.05, 0.1, 0.2, 0.5, 1.0 m/s
    target.left_speed = 0.5;   // m/s
    target.right_speed = 0.5;  // m/s

    hw.left_encoder.reset_ticks();
    hw.right_encoder.reset_ticks();

    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.left_encoder, hw.encoder_sample_us);

    while (1)
    {
        /// TEST SEQUENCE: 1. Sample encoders
        ///                2. Update PID with exact dummy target speeds and the measured encoder speeds
        ///                3. Convert PID output to PWM duty cycle and set motor speeds

        EncoderInput sample_encoder = Sample::sample_encoders(
            hw.left_encoder, hw.right_encoder, encoder_timing);

        float dt_sec = encoder_timing.sample_time_sec;
        pid.update(sample_encoder, target, dt_sec, output);

        pid.output_to_pwm_duty_cycle(output, left_pwm, right_pwm);

        hw.left_motor.set_speed(left_pwm);
        hw.right_motor.set_speed(right_pwm);
    }

    return 0;
}
