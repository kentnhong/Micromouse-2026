#include "trapezoidal.h"

#include <algorithm>
#include <cmath>

namespace MM
{

bool Trapezoidal::configure(float min_speed_ticks, float max_speed_ticks,
                            float accel_ticks_per_sec2,
                            float decel_ticks_per_sec2,
                            int32_t total_distance_ticks)
{
    min_speed = std::max(0.0f, min_speed_ticks);
    max_speed = std::max(min_speed, max_speed_ticks);
    accel_rate = std::max(0.0f, accel_ticks_per_sec2);
    decel_rate = std::max(0.0f, decel_ticks_per_sec2);
    distance_goal_ticks = std::max<int32_t>(0, total_distance_ticks);

    return reset();
}

bool Trapezoidal::reset()
{
    // Start at min speed so the first few PID updates ramp smoothly.
    speed_setpoint = min_speed;
    progress_ticks = 0;
    complete = false;

    return true;
}

bool Trapezoidal::trapezoidal(const EncoderInput& input, float dt_sec)
{
    if (dt_sec <= 0.0f)
    {
        return false;
    }

    if (complete)
    {
        return true;
    }

    // Use average absolute wheel delta to estimate forward progress.
    const int32_t left_abs = std::abs(input.left_ticks);
    const int32_t right_abs = std::abs(input.right_ticks);
    const int32_t delta_progress = (left_abs + right_abs) / 2;
    progress_ticks += delta_progress;

    // Distance-aware profile: accelerate, then decelerate when remaining
    // distance falls below stopping distance for current speed.
    if (distance_goal_ticks > 0)
    {
        const int32_t remaining_ticks =
            std::max<int32_t>(0, distance_goal_ticks - progress_ticks);

        float decel_distance = 0.0f;
        if (decel_rate > 0.0f)
        {
            decel_distance =
                (speed_setpoint * speed_setpoint - min_speed * min_speed) /
                (2.0f * decel_rate);
        }

        if (static_cast<float>(remaining_ticks) <= decel_distance)
        {
            speed_setpoint =
                std::max(min_speed, speed_setpoint - decel_rate * dt_sec);
        }
        else
        {
            speed_setpoint =
                std::min(max_speed, speed_setpoint + accel_rate * dt_sec);
        }

        if (progress_ticks >= distance_goal_ticks &&
            speed_setpoint <= (min_speed + 1.0f))
        {
            speed_setpoint = min_speed;
            complete = true;
        }

        return true;
    }

    // Ramp-only mode (no distance goal): accelerate up to max and mark
    // complete once the profile has reached cruise speed.
    speed_setpoint = std::min(max_speed, speed_setpoint + accel_rate * dt_sec);
    if (speed_setpoint >= max_speed)
    {
        speed_setpoint = max_speed;
        complete = true;
    }

    return true;
}

}  // namespace MM