/**
* @file pid.h
* @author Bex Saw
* @brief Generic velocity PID controller class definition
* @version 0.1
*/

#pragma once

#include <cstdint>
#include "drv8231.h"
#include "pid_math.h"

namespace MM
{
class PID
{
public:
    /**
    * @brief PID controller wrapping an individual motor
    */
    PID(Drv8231& motor, const Gains& gains);

    /**
    * @brief Update the PID from an encoder tick measurement
    * @param desired_speed_ticks Desired velocity in ticks/s.
    * @param polarity Direction of the motor command (FORWARD or REVERSE)
    * @param measured_ticks Measured delta ticks over the sampling period.
    * @param dt_sec Time step for the update in seconds.
    */
    bool update(float desired_speed_ticks, Drv8231::Direction polarity,
                int32_t measured_ticks, float dt_sec);

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

    Drv8231& motor;
    Gains gains{};
    Context state{};
    float min_output{-1.0f};
    float max_output{1.0f};
    float integral_limit{1000.0f};
};
}  // namespace MM
