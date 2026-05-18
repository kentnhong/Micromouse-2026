#include "pid.h"
#include <algorithm>
#include <tuple>

namespace MM
{

PID::PID(Drv8231& motor, const Gains& gains)
    : motor(motor),
      gains(gains),
      min_output(-1.0f),
      max_output(1.0f),
      integral_limit(1000.0f)
{
}

bool PID::update(float desired_speed_ticks, Drv8231::Direction polarity,
                 int32_t measured_ticks, float dt_sec)
{
    if (dt_sec <= 0.0f)
    {
        motor.drive(Drv8231::Direction::COAST, 0);
        return false;
    }

    if (polarity != Drv8231::Direction::FORWARD &&
        polarity != Drv8231::Direction::REVERSE)
    {
        motor.drive(Drv8231::Direction::COAST, 0);
        return false;
    }

    // 1. Calculate controller output based on difference
    float error = desired_speed_ticks - (measured_ticks / dt_sec);
    float output = compute_pid(error, dt_sec);

    // 2. Sum the signals and constrain proportional result
    float final_drive = limit_range(output, min_output, max_output);

    // 3. Map [-1.0, 1.0] float to Polarity + 0-100% Duty Cycle
    Drv8231::Direction dir = polarity;

    if (final_drive == 0.0f)
    {
        motor.drive(Drv8231::Direction::COAST, 0);
        return true;
    }

    if (final_drive < 0.0f)
    {
        // Reverse whatever the desired polarity was
        dir = (polarity == Drv8231::Direction::FORWARD)
                  ? Drv8231::Direction::REVERSE
                  : Drv8231::Direction::FORWARD;
    }

    // Map magnitude to 0-100 duty cycle
    uint8_t duty_cycle = static_cast<uint8_t>(std::abs(final_drive) * 100.0f);
    if (duty_cycle == 0)
    {
        motor.drive(Drv8231::Direction::COAST, 0);
        return true;
    }

    // 4. Send to motor driver directly
    motor.drive(dir, duty_cycle);

    return true;
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
