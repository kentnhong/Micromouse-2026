/**
 * @file pid_math.h
 * @brief Common math types for PID controller
 * @author Bex Saw
 * @date 3/28/2026
 */

#pragma once

namespace MM
{

/**
 * @brief Struct to hold PID gains values for the PID controller
 * P - Proportional term: based on current error
 * I - Integral term: based on accumulated error over time
 * D - Derivative term: based on rate of change of error
 */
struct Val
{
    float kp, ki, kd;
};

/**
 * @brief Struct to hold the current error values for the PID controller
 * kp_err - current proportional error
 * ki_error - accumulated integral error
 * kd_error - current derivative error
*/
struct Error
{
    float kp_err, ki_error, kd_error;
};

}  // namespace MM