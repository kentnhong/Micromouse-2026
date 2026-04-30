/**
* @file pid.h
* @author Bex Saw
* @brief Generic velocity PID controller class definition
* @version 0.1
*/

#pragma once

#include <cstdint>
#include "pid_math.h"

namespace MM
{
class PID
{
public:
    /**
    * @brief PID controller configuration parameters
    * @param gains PID gains (Kp, Ki, Kd)
    * @param min_output Minimum output value for the controller (e.g., -1.0 for motor drive)
    * @param max_output Maximum output value for the controller (e.g., 1.0 for motor drive)
    * @param integral_limit Maximum absolute value for the integral term to prevent windup
    */
    struct PIDConfig
    {
        Gains gains;
        float min_output{-1.0f};
        float max_output{1.0f};
        float integral_limit{1000.0f};
    };

    explicit PID(const PIDConfig& config);

    /**
    * @brief Update the PID from a measured velocity in encoder ticks per second.
    * @param measured_ticks_per_sec Current measured velocity in ticks/s.
    * @param target_ticks_per_sec Target velocity in ticks/s.
    * @param dt_sec Time step for the update in seconds.
    * @param output Normalized motor command in the configured output range.
    */
    bool update(float measured_ticks_per_sec, float target_ticks_per_sec,
                float dt_sec, float& output);

    /**
    * @brief Update the PID from encoder ticks measured over the sample period.
    * @param measured_ticks Encoder delta ticks for this control period.
    * @param target_ticks_per_sec Target velocity in ticks/s.
    * @param dt_sec Time step for the update in seconds.
    * @param output Normalized motor command in the configured output range.
    */
    bool update_ticks(int32_t measured_ticks, float target_ticks_per_sec,
                      float dt_sec, float& output);

    /**
    * @brief Reset the PID controller state.
    */
    bool reset();

    /**
    * @brief Set output clamp range, typically -1.0 to 1.0 for motor drive.
    */
    bool set_output_limits(float min_output, float max_output);

    /**
    * @brief Set integral clamp range.
    */
    bool set_integral_limit(float integral_limit);

    /**
    * @brief Store a target velocity value in ticks/s.
    */
    bool set_target_ticks_per_sec(float ticks_per_sec, float& target);

    /**
    * @brief Convert encoder delta ticks to ticks/s.
    */
    bool ticks_to_ticks_per_second(int32_t ticks, float dt_sec,
                                   float& ticks_per_sec);

private:
    /**
    * @brief Context struct 
    * This is for storing internal state of the PID controller, 
    * with integral & prev_error for the I and D terms, respectively.
    */
    struct Context
    {
        float integral = 0.0f;
        float prev_error = 0.0f;
    };

    float compute_pid(float error, float dt_sec);
    static float limit_range(float value, float min_value, float max_value);

    Gains gains{};
    Context state{};
    float min_output{-1.0f};
    float max_output{1.0f};
    float integral_limit{1000.0f};
};
}  // namespace MM
