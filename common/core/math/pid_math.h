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
struct Gains
{
    float kp, ki, kd;
};

}  // namespace MM