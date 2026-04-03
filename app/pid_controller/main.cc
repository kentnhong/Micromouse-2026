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

/**
* @note For this test only care about the PID controller and its output to the motor driver.
* The trapezoidal velocity profile is not used in this test, but will be part of the motion 
* API test later on. Once this is verified.
*/
int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    auto [pid, input, output, target] = get_pid_bundle();

    // PWM duty cycles for left and right motors
    uint8_t left_pwm = 0;
    uint8_t right_pwm = 0;

    hw.encoder.reset_ticks();
    hw.drv8231.set_direction(Drv8231::Direction::FORWARD);

    while (1)
    {
        int32_t ticks = hw.encoder.get_ticks();

        input.encoder.left_ticks = ticks;
        input.encoder.right_ticks = ticks;

        hw.imu.read_all(input.imu);  // yaw

        pid.update(input, target, 0.0f, output);

        pid.output_to_pwm_duty_cycle(output, left_pwm, right_pwm);

        /* Average the left and right PWM values for a single duty cycle 
           since the driver controls both motors together */
        const uint8_t duty_cycle =
            static_cast<uint8_t>((left_pwm + right_pwm) / 2);

        hw.drv8231.set_speed(duty_cycle);

        hw.encoder.reset_ticks();
        Utils::DelayMs(10);
    }

    return 0;
}