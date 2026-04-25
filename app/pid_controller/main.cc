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
#include "trapezoidal.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();
    Motion motion(hw);

    auto [pid, output, target] = get_pid_bundle();
    Trapezoidal profile;

    uint8_t left_pwm = 0;
    uint8_t right_pwm = 0;

    hw.left_encoder.reset_ticks();
    hw.right_encoder.reset_ticks();

    while (1)
    {
        /// INSIDE MOTION SEQUENCE: 1. Sample encoders
        ///                         2. Update trapezoidal profile to get new setpoints
        ///                         3. Update PID with new setpoints and encoder readings to get new motor outputs
        ///                         4. Convert PID output to PWM duty cycle and set motor speeds
        motion.update();

        pid.output_to_pwm_duty_cycle(output, left_pwm, right_pwm);

        hw.left_motor.set_speed(left_pwm);
        hw.right_motor.set_speed(right_pwm);
    }

    return 0;
}
