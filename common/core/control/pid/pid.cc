#include "pid.h"
#include <algorithm>
#include <tuple>

namespace MM
{

PID::PID(const PIDConfig& config)
    : gains(config.gains),
      min_output(config.min_output),
      max_output(config.max_output),
      integral_limit(config.integral_limit)
{
}

bool PID::update(float measured_ticks_per_sec, float target_ticks_per_sec,
                 float dt_sec, float& output)
{
    if (dt_sec <= 0.0f)
    {
        output = 0.0f;
        return false;
    }

    // Error for velocity PID is in ticks/s, and output is normalized motor drive (-1 to 1)
    const float error = target_ticks_per_sec - measured_ticks_per_sec;

    // Output example: -1 to 1 for motor drive
    output = compute_pid(error, dt_sec);

    return true;
}

bool PID::update_ticks(int32_t measured_ticks, float target_ticks_per_sec,
                       float dt_sec, float& output)
{
    float measured_ticks_per_sec = 0.0f;

    // If ticks_to_ticks_per_second fails it feed back 0
    if (!ticks_to_ticks_per_second(measured_ticks, dt_sec,
                                   measured_ticks_per_sec))
    {
        output = 0.0f;
        return false;
    }

    return update(measured_ticks_per_sec, target_ticks_per_sec, dt_sec, output);
}

bool PID::reset()
{
    state = Context{};
    return true;
}

bool PID::set_output_limits(float min_output, float max_output)
{
    std::tie(this->min_output, this->max_output) =
        std::minmax(min_output, max_output);

    return true;
}

bool PID::set_integral_limit(float integral_limit)
{
    this->integral_limit = integral_limit;

    return true;
}

bool PID::set_target_ticks_per_sec(float ticks_per_sec, float& target)
{
    target = ticks_per_sec;
    return true;
}

bool PID::ticks_to_ticks_per_second(int32_t ticks, float dt_sec,
                                    float& ticks_per_sec)
{
    if (dt_sec <= 0.0f)
    {
        ticks_per_sec = 0.0f;
        return false;
    }

    ticks_per_sec = static_cast<float>(ticks) / dt_sec;
    return true;
}

float PID::compute_pid(float error, float dt_sec)
{
    const float p = gains.kp * error;

    state.integral += error * dt_sec;
    state.integral =
        limit_range(state.integral, -integral_limit, integral_limit);
    const float i = gains.ki * state.integral;

    const float derivative = (error - state.prev_error) / dt_sec;
    const float d = gains.kd * derivative;

    state.prev_error = error;

    return limit_range(p + i + d, min_output, max_output);
}

float PID::limit_range(float value, float min_value, float max_value)
{
    // Range from min_output to max_output, typically -1.0 to 1.0 for motor drive
    value = std::clamp(value, min_value, max_value);

    return value;
}

}  // namespace MM
