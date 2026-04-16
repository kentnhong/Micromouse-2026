/**
* @file main.cc
* @brief PID controller main application
* @author Bex Saw
* @date 3/31/2026
*/

#include <tuple>
#include "board.h"
#include "pid.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    auto [pid, output, target] = get_pid_bundle();

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
        EncoderInput encoder = Sample::sample_encoders(
            hw.left_encoder, hw.right_encoder, encoder_timing);

        // Update the PID controller with the encoder reading and target velocity
        if (!pid.update(encoder, target, encoder_timing.sample_time_sec,
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
