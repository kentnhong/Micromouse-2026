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

/*
 - Input:
 ** Logs + run history of multiple runs (for tuning and debugging)
 - Output: control signal: motor power, steering angle, etc.
*/

namespace MM
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

class PID
{

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
    void set_output_limits(float min_output, float max_output);

    /**
     * @brief Set integral clamp range
     */
    void set_integral_limit(float integral_limit);


private:

    // Integral stores accumulated error for I term
    // prev_error stores last error for D term  
    struct State
    {
        float integral = 0.0f;
        float prev_error = 0.0f; 
    };

    // Separate PID loops for speed and turning control
    struct Loop
    {
        Val pid{};
        State state;
    };

    float compute_pid(Loop& loop, float error, float dt_sec);

    // This is need for integral windup prevention and to ensure output is within motor limits

    // Range from -100 to 100 for motor power percentage
    static float limit_range(float value, float min_value, float max_value);

    // Normalize angle to [-180, 180] degrees for consistent error calculation
    static float limit_angle(float angle_deg);

    Loop speed{};
    Loop turn{};

    // Output limits for motor control signal
    float min_output{-100.0f};
    float max_output{100.0f};
    float integral_limit{1000.0f};

};
}  // namespace MM