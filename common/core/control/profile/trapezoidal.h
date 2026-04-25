/**
* @file trapezoidal.h
* @author Bex Saw
* @brief Helper function for trapezoidal velocity profile
* @version 0.1
*/

/**
* @note Generate dynamic speed setpoints based on progress.
* While, PID follows those setpoints as closely as possible
* It helps with smoothing the accel & decel phases.
*/

#pragma once
#include "enc_math.h"

namespace MM
{
class Trapezoidal
{
public:
    struct VelocitySetpoint
    {
        float left;
        float right;
    };

    /**
    * @brief Generates velocity setpoints for a trapezoidal velocity profile
    * @param target The target distance to travel (in mm)
    * @param encoder The current encoder readings for both wheels
    * @param ticks_to_mm Conversion factor from encoder ticks to millimeters
    * @param turn Whether the robot is turning (1 for turn, 0 for straight)
    * @return A boolean indicating whether the target distance has been reached within a threshold
    */
    VelocitySetpoint trapezoidal(float target, const EncoderInput& encoder,
                                 float ticks_to_mm, uint8_t turn);

    bool is_complete() const
    {
        return complete;
    }

private:
    float max_speed{100.0f};                // [mm/s]
    float min_speed{20.0f};                 // [mm/s]
    float acceleration_distance{100.0f};    // [mm]
    float target_distance_threshold{1.0f};  // [mm]
    bool complete{false};

    // Need to change all of this later based on testing and tuning
};
}  // namespace MM
