/**
* @file trapezoidal.h
* @author Bex Saw
* @brief Helper function for trapezoidal velocity profile
* @version 0.1
*/

#pragma once
#include <cmath>
#include "enc_math.h"

/**
* @note This profile taking in the ticks/s and then outputting the desired ticks/s for the left and right motors.
*       The trapezoidal profile will have a max speed, min speed, and acceleration/deceleration rate that can be tuned.
*/
namespace MM
{
class Trapezoidal
{
public:
    /**
    * @brief Struct to hold the desired velocity setpoint for the left and right motors.
    */
    struct EncoderInput
    {
        int32_t left = 0;
        int32_t right = 0;
    };

    // TODO: Need to change params this based on the ticks/s and max speed and min for the decel/accel
    bool trapezoidal(const EncoderInput& input, float dt_sec);

    bool is_complete() const
    {
        return complete;
    }

private:
    // ticks/s for left and right motors
    float max_speed{};
    float min_speed{0};

    bool complete{false};
};
}  // namespace MM
