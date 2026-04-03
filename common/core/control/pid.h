/**
* @file pid.h
* @author Bex Saw
* @brief PID controller class definition
* @version 0.1
*/

#pragma once

#include <cstdint>
#include "bno055_imu.h"
#include "elapsed.h"
#include "enc_math.h"
#include "pid_math.h"
#include "trapezoidal.h"

/**
* @note No derivative filtering (can be sensitive to sensor noise).
*       No output clamping on the final motor command (only on integral).
*       No anti-windup for output saturation.
*       No advanced features like feedforward, adaptive gains, or state estimation.
*/
namespace MM
{
class PID
{
public:
    /**
* @brief The PID controller inputs (IMU (gyro) and encoders) 
* @param encoder The encoder input values
* @param imu The IMU input values
*/
    struct Input
    {
        EncoderInput encoder;
        Bno055Data imu;
    };

    struct Target
    {
        float left_speed;
        float right_speed;
        float yaw;  // desired yaw angle (deg)
    };

    struct MotorOutput
    {
        float left, right;
    };

    struct PIDConfig
    {
        Val left;
        Val right;
        Val yaw;
    };

    explicit PID(const PIDConfig& config);

    /**
    * @brief Update the PID controller with the current measured value and return the control signal
    * @param input The current sensor readings (IMU and encoder data)
    * @param target The target values for distance and angle
    * @param dt_sec The time step for the update (in seconds) 
    * @param output The output control signals for the motors
    */
    bool update(const Input& input, const Target& target, float dt_sec,
                MotorOutput& output);

    /**
    * @brief Reset the PID controller 
    */
    bool reset();

    /**
     * @brief Convert encoder ticks to velocity (m/s) using wheel circumference and ticks per revolution
     * @param encoder The encoder input values (ticks)
     * @param left_velocity The resulting left wheel velocity in m/s (output parameter)
     * @param right_velocity The resulting right wheel velocity in m/s (output parameter)
     */
    bool ticks_to_velocity(const EncoderInput& encoder, float& left_velocity,
                           float& right_velocity);

    /**
     * @brief Set motor output clamp range
     * @param min_output The minimum output value (e.g., -100 for full reverse)
     * @param max_output The maximum output value (e.g., 100 for full forward)
     */
    bool set_output_limits(float min_output, float max_output);

    /**
     * @brief Set integral clamp range
     * @param integral_limit The maximum absolute value for the integral term to prevent windup
     */
    bool set_integral_limit(float integral_limit);

    /**
    * @brief Convert PID output to motor duty cycle (percentage)
    * @param output The PID output control signals for the motors
    * @param left_duty The resulting duty cycle for the left motor (output parameter)
    * @param right_duty The resulting duty cycle for the right motor (output parameter)
    */
    bool output_to_duty_cycle(const MotorOutput& output, float& left_duty,
                              float& right_duty);

    /**
    * @brief Convert PID output to clamped PWM duty cycle percentages
    * @param output The PID output control signals for the motors
    * @param left_pwm The resulting left motor PWM duty cycle (0-100)
    * @param right_pwm The resulting right motor PWM duty cycle (0-100)
    */
    bool output_to_pwm_duty_cycle(const MotorOutput& output, uint8_t& left_pwm,
                                  uint8_t& right_pwm);

private:
    // Integral stores accumulated error for Integral term
    // prev_error stores last error for Derivative term
    struct Context
    {
        float integral = 0.0f;    // I: Accumulated error (Integral term)
        float prev_error = 0.0f;  // D: Previous error (Derivative term)
    };

    // Separate PID loops for left, right wheels, and yaw
    struct Loop
    {
        Val pid{};
        Context state;
    };

    // Computes PID output using P, I, D terms
    float compute_pid(Loop& loop, float error, float dt_sec);

    // Range from -100 to 100 for motor power percentage
    static float limit_range(float value, float min_value, float max_value);
    static uint8_t clamp_duty_cycle(float duty_cycle);

    static float limit_angle(float angle);

    Loop left{};   // PID loop for left wheel
    Loop right{};  // PID loop for right wheel
    Loop yaw{};    // PID loop for yaw (heading)

    // Output limits for motor control signal
    float min_output{-100.0f};
    float max_output{100.0f};
    float integral_limit{1000.0f};
};
}  // namespace MM