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
    float p, i, d;
};

/**
 * @brief Struct to hold the current error values for the PID controller
 * p_err - current proportional error
 * i_error - accumulated integral error
 * d_error - current derivative error
*/
struct Error
{
    float p_err, i_error, d_error;
};

}  // namespace MM