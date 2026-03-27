/**
* @file pid.h
* @author Bex Saw
* @brief PID controller class definition
* @version 0.1
*/

#pragma once
#include <cstdint>

/*
 - Input: target value: desired distance, angle, or speed
 - Input: Measured value: actual distance, angle, or speed
          from sensors: encoder counts, utils/systicks, imu for angle (gyro), etc.
 ** External Flash will have saved params (PID constants, calibration data, etc.)
 ** Logs + run history of multiple runs (for tuning and debugging)
 - Output: control signal: motor power, steering angle, etc.
*/

namespace MM
{
class PID
{
public:
    explicit PID(float kp, float ki, float kd);

    // Straight line movement - distance from encoders

    // Turning - angle from gyro

    // Speed control - speed from encoders or IMU

    // Update the PID controller with the current measured value and return the control signal

    // Reset the PID controller (e.g., when starting a new movement or after reaching the target)

    // Set new PID constants (e.g., for tuning)

    // Get current PID constants (e.g., for logging or debugging) from flash

    // Get current error values (e.g., for logging or debugging)

    // etc. 

private:
    // constants
    
};
}