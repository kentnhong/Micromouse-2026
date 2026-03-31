/**
* @file main.cc
* @brief PID controller main application
* @author Bex Saw
* @date 3/31/2026
*/

<<<<<<< HEAD
<<<<<<< HEAD
#include <tuple>
=======
>>>>>>> 983c61d (pid bsp + main.cc)
=======
#include <tuple>
>>>>>>> 0867819 (PID accessor)
#include "board.h"
#include "pid.h"

using namespace MM;

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 383affe (Trapezoidal; Generate dynamic speed setpoints based on progress)
/**
* @note For this test only care about the PID controller and its output to the motor driver.
* The trapezoidal velocity profile is not used in this test, but will be part of the motion 
* API test later on. Once this is verified.
*/
<<<<<<< HEAD
=======
constexpr Val kWheelPid{250.0f, 5.0f, 0.0f};
constexpr Val kYawPid{0.0f, 0.0f, 0.0f};
constexpr float kTargetSpeedMps = 0.10f;
constexpr float kLoopDtSec = 0.01f;
constexpr uint32_t kLoopDelayMs = 10;
=======
constexpr Val kWheelPid{250.0f, 5.0f, 0.0f}; // (P, I, D) gains for wheel speed control - these will need to be tuned experimentally
constexpr Val kYawPid{0.0f, 0.0f, 0.0f}; // (P, I, D) gains for yaw control - set to zero for now since we're not controlling yaw in this test

constexpr float kTargetSpeedMps = 0.10f; // This need to be replace based on tuning + speed testing
constexpr float kLoopDtSec = 0.01f; // 10 ms loop time for ~100 Hz control loop
constexpr uint32_t kLoopDelayMs = 10; // Delay based on the loop time in milliseconds
>>>>>>> b74cd35 (notes on PID val)
=======
constexpr Val kWheelPid{
    250.0f, 5.0f,
    0.0f};  // (P, I, D) gains for wheel speed control - these will need to be tuned experimentally
constexpr Val kYawPid{
    0.0f, 0.0f,
    0.0f};  // (P, I, D) gains for yaw control - set to zero for now since we're not controlling yaw in this test

constexpr float kTargetSpeedMps =
    0.10f;  // This need to be replace based on tuning + speed testing
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)

>>>>>>> 983c61d (pid bsp + main.cc)
=======
>>>>>>> 0867819 (PID accessor)
=======
>>>>>>> 383affe (Trapezoidal; Generate dynamic speed setpoints based on progress)
int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    auto [pid, input, output, target] = get_pid_bundle();
=======
    // left, right, yaw
    PID pid(kWheelPid, kWheelPid, kYawPid);
=======
    // We could make another file or throw it in BSP (status: TBA)
    PID::PIDConfig config{
        .left = kWheelPid,
        .right = kWheelPid,
        .yaw = kYawPid,
    };

    PID pid(config);
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)

    // Inputs
    PID::Input input{};
    PID::MotorOutput output{};

    // speed, yaw rate, and yaw angle targets
    PID::Target target{kTargetSpeedMps, kTargetSpeedMps, 0.0f};
=======
    auto [pid, input, output, target] = get_pid_bundle();
>>>>>>> 0867819 (PID accessor)

<<<<<<< HEAD
    // PID outputs
    PID::MotorOutput output{};
>>>>>>> 983c61d (pid bsp + main.cc)

=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
    // PWM duty cycles for left and right motors
    uint8_t left_pwm = 0;
    uint8_t right_pwm = 0;

<<<<<<< HEAD
<<<<<<< HEAD
    hw.encoder.reset_ticks();
=======
    // Reset encoder ticks and initialize motor driver
    hw.encoder.reset_ticks();
    hw.drv8231.init();

    // Set initial motor direction (e.g., forward)
>>>>>>> 983c61d (pid bsp + main.cc)
=======
    hw.encoder.reset_ticks();
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
    hw.drv8231.set_direction(Drv8231::Direction::FORWARD);

    while (1)
    {
<<<<<<< HEAD
<<<<<<< HEAD
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
=======
        // This board currently exposes one encoder and one motor driver,
        // so feed the same wheel measurement into both PID wheel channels.
=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
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
<<<<<<< HEAD
        Utils::DelayMs(kLoopDelayMs);
>>>>>>> 983c61d (pid bsp + main.cc)
=======
        Utils::DelayMs(10);
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
    }

    return 0;
}
