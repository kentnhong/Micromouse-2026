/**
* @file elapsed.h
* @brief Helper function to compute elapsed time in seconds
* @author Bex Saw
* @date 3/31/2026
*/

/**
* @note The benefit of having the elapsed time is to allow the PID controller to compute the time step (dt) between updates, 
* which is crucial for accurate Integral and Derivative calculations. This is especially important if the control loop does 
* not run at a fixed frequency, as it allows the PID controller to adapt to varying update rates.
*/

#pragma once

#include <cstdint>
#include "delay.h"

namespace MM
{
class Elapsed
{
public:
<<<<<<< HEAD
<<<<<<< HEAD

/**
=======
    /**
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
=======

/**
>>>>>>> f98ad1e (clangd conflict)
 * @brief Compute elapsed time in seconds since the last call to this function
 * @return Elapsed time in seconds
 */
    static float get_dt_sec();

private:
    uint32_t now;  // Stores the current timestamp in milliseconds
    float dt_sec;  // Stores the computed elapsed time in seconds
};
}  // namespace MM