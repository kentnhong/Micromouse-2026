/**
* @file pid.h
* @author Bex Saw
* @brief PID controller class definition
* @version 0.1
*/

#pragma once

#include <cstdint>
#include "imu_math.h"
#include "pid_math.h"
#include "enc_math.h"

namespace MM
{
class PID
{

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
    float speed = 0.0f;
    float yaw = 0.0f;
};

struct MotorOutput
{
    float left = 0.0f;
    float right = 0.0f;
};

public:
    // Constructor fetch all inputs + PID constants from flash
    explicit PID(const Val& speed, const Val& turn);
    /**
    * @brief Update the PID controller with the current measured value and return the control signal
    * @param input The current sensor readings (IMU and encoder data)
    * @param target The target values for distance and angle
    * @param dt_sec The time step for the update (in seconds) 
    * @param output The output control signals for the motors
    */
    bool update(const Input& input, const Target& target, float dt_sec, MotorOutput& output);

    /**
    * @brief Reset the PID controller 
    */
    bool reset();

    /**
     * @brief Set motor output clamp range
     */
    bool set_output_limits(float min_output, float max_output);

    /**
     * @brief Set integral clamp range
     */
    bool set_integral_limit(float integral_limit);


private:

    // Integral stores accumulated error for Integral term
    // prev_error stores last error for Derivative term  
    struct State
    {
        float integral = 0.0f;   // I: Accumulated error (Integral term)
        float prev_error = 0.0f; // D: Previous error (Derivative term)
    };

    // Separate PID loops for speed and turning control
    struct Loop
    {
        Val pid{};      // Contains Kp (P), Ki (I), Kd (D) gains
        State state;    // Holds integral and previous error for I and D terms
    };

    // Computes PID output using P, I, D terms
    float compute_pid(Loop& loop, float error, float dt_sec);

    // This is need for integral windup prevention and to ensure output is within motor limits

    // Range from -100 to 100 for motor power percentage
    static float limit_range(float value, float min_value, float max_value);

    // Normalize angle to [-180, 180] degrees for consistent error calculation
    static float limit_angle(float angle_deg);

    Loop speed{}; // PID loop for speed control (P, I, D all used)
    Loop turn{};  // PID loop for turning/yaw control (P, I, D all used)

    // Output limits for motor control signal
    float min_output{-100.0f};
    float max_output{100.0f};
    float integral_limit{1000.0f};

};
}  // namespace MM