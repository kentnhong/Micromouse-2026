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
}  // namespace MM