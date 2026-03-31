/**
* @file pid.h
* @author Bex Saw
* @brief PID controller class definition
* @version 0.1
*/

#pragma once

#include <cstdint>
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
#include "bno055_imu.h"
#include "elapsed.h"
#include "enc_math.h"
#include "pid_math.h"
#include "trapezoidal.h"
<<<<<<< HEAD

/**
* @note No derivative filtering (can be sensitive to sensor noise).
*       No output clamping on the final motor command (only on integral).
*       No anti-windup for output saturation.
*       No advanced features like feedforward, adaptive gains, or state estimation.
=======
=======
>>>>>>> efd260e (PID control adding init)
=======
>>>>>>> 4895d09 (PID structure + ideas)
=======
>>>>>>> f506e16 (PID functions + expected math)
#include "imu_math.h"
#include "pid_math.h"
#include "enc_math.h"

<<<<<<< HEAD
<<<<<<< HEAD
/*
 - Input:
 ** Logs + run history of multiple runs (for tuning and debugging)
 - Output: control signal: motor power, steering angle, etc.
>>>>>>> 3da8257 (PID structure + ideas)
*/
=======
>>>>>>> 43d9502 (PID math implemented; still need to verify)
=======
=======
#include "bno055_imu.h"
#include "elapsed.h"
#include "enc_math.h"
#include "pid_math.h"
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
=======
>>>>>>> 383affe (Trapezoidal; Generate dynamic speed setpoints based on progress)

/**
* @note No derivative filtering (can be sensitive to sensor noise).
*       No output clamping on the final motor command (only on integral).
*       No anti-windup for output saturation.
*       No advanced features like feedforward, adaptive gains, or state estimation.
*/
<<<<<<< HEAD

>>>>>>> cd5cf28 (note on limitation)
=======
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
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

<<<<<<< HEAD
<<<<<<< HEAD
public:
<<<<<<< HEAD
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

=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
    struct PIDConfig
    {
        Val left;
        Val right;
        Val yaw;
    };

    explicit PID(const PIDConfig& config);

<<<<<<< HEAD
    /**
    * @brief Update the PID controller with the current measured value and return the control signal
    * @param input The current sensor readings (IMU and encoder data)
    * @param target The target values for distance and angle
    * @param dt_sec The time step for the update (in seconds) 
    * @param output The output control signals for the motors
    */
    bool update(const Input& input, const Target& target, float dt_sec,
                MotorOutput& output);

=======
=======
>>>>>>> 983c61d (pid bsp + main.cc)
    // Constructor fetch all inputs + PID constants from flash
    explicit PID(const Val& left, const Val& right, const Val& yaw);
=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
    /**
    * @brief Update the PID controller with the current measured value and return the control signal
    * @param input The current sensor readings (IMU and encoder data)
    * @param target The target values for distance and angle
    * @param dt_sec The time step for the update (in seconds) 
    * @param output The output control signals for the motors
    */
    bool update(const Input& input, const Target& target, float dt_sec,
                MotorOutput& output);

>>>>>>> 3da8257 (PID structure + ideas)
    /**
    * @brief Reset the PID controller 
    */
    bool reset();
<<<<<<< HEAD

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
=======

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
<<<<<<< HEAD
<<<<<<< HEAD
>>>>>>> 3da8257 (PID structure + ideas)
=======
=======
class PID
=======
>>>>>>> 287ef60 (PID structure + ideas)
{
    EncoderInput encoder;
    Bno055Data imu;
};

struct Output
{
    float left_motor_output, right_motor_output;
};

class PIDMath : public PID
{

public:
    // Constructor fetch all inputs + PID constants from flash
    explicit PID(Bno055Data& imu_data, EncoderInput& enc_data, Val& pid);

    /**
    * @brief Update the PID controller with the current measured value and return the control signal
    * @param input The current sensor readings (IMU and encoder data)
    * @param target The target values for distance and angle
    * @param output The output control signals for the motors
    */
    bool update(const Input& input, const Target& target, Output& output);

    /**
    * @brief Reset the PID controller 
    */
    bool reset();

private:
    /**
    * @brief Set the output limits for the PID controller 
    * @param min_output The minimum output value (e.g., -100 for full reverse)
    * @param max_output The maximum output value (e.g., 100 for full forward)
    */
    bool set_output_limits(float min_output, float max_output);

    // PID values and error values
    Val pid_val;
    Error pid_error;
};
<<<<<<< HEAD
}
>>>>>>> 4bebdbe (PID control adding init)
<<<<<<< HEAD
>>>>>>> efd260e (PID control adding init)
=======
=======
}  // namespace MM
>>>>>>> 287ef60 (PID structure + ideas)
>>>>>>> 4895d09 (PID structure + ideas)
=======
>>>>>>> f506e16 (PID functions + expected math)
