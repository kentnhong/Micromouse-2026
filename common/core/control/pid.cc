#include "pid.h"
#include <cmath>
#include <numbers>

namespace MM
{

static constexpr float kMaxAngle = 180.0f;
static constexpr float kWheelCircumference =
    std::numbers::pi * 0.0381f;  // 1.5 inch diameter wheel => C = pi * d
static constexpr float kTicksPerRevolution =
    12 * 4;  // 12 ticks per revolution from encoder, x4 for quadrature encoding

PID::PID(const PIDConfig& config)
{
    this->left.pid = config.left;
    this->right.pid = config.right;
    this->left.state = Context{};
    this->right.state = Context{};
}

bool PID::update(const EncoderInput& encoder, const Target& target,
                 float dt_sec, MotorOutput& output)
{
    // Convert encoder ticks to velocity (m/s)
    float left_velocity = 0.0f;
    float right_velocity = 0.0f;
    ticks_to_velocity(encoder, left_velocity, right_velocity);

    // Per-wheel PID: calculate error for each wheel (no yaw)
    float left_error = target.left_speed - left_velocity;
    float right_error = target.right_speed - right_velocity;

    float left_output = compute_pid(this->left, left_error, dt_sec);
    float right_output = compute_pid(this->right, right_error, dt_sec);

    output.left = left_output;
    output.right = right_output;

    return true;
}

// float PID::limit_angle(float angle)
// {
//     while (angle > 180.0f) angle -= 360.0f;
//     while (angle < -180.0f) angle += 360.0f;
//     return angle;
// }

bool PID::reset()
{
    this->left.state = Context{};
    this->right.state = Context{};
    return true;
}

bool PID::set_output_limits(float min_output, float max_output)
{
    this->min_output = min_output;
    this->max_output = max_output;

    return true;
}

bool PID::set_integral_limit(float integral_limit)
{
    this->integral_limit = integral_limit;

    return true;
}

bool PID::output_to_duty_cycle(const MotorOutput& output, float& left_duty,
                               float& right_duty)
{
    // Convert PID output to duty cycle percentage
    left_duty = limit_range(output.left, min_output, max_output);
    right_duty = limit_range(output.right, min_output, max_output);

    return true;
}

bool PID::output_to_pwm_duty_cycle(const MotorOutput& output, uint8_t& left_pwm,
                                   uint8_t& right_pwm)
{
    float left_duty = 0.0f;
    float right_duty = 0.0f;
    output_to_duty_cycle(output, left_duty, right_duty);

    left_pwm = clamp_duty_cycle(left_duty);
    right_pwm = clamp_duty_cycle(right_duty);

    return true;
}

float PID::compute_pid(Loop& loop, float error, float dt_sec)
{

    // Proportional
    // P = kp * error
    float P = loop.pid.kp * error;

    // Integral
    // I = ki * integral(error * dt)
    loop.state.integral += error * dt_sec;
    loop.state.integral =
        limit_range(loop.state.integral, -integral_limit, integral_limit);
    float I = loop.pid.ki * loop.state.integral;

    // Derivative
    // D = kd * (error - prev_error) / dt
    float derivative = (error - loop.state.prev_error) / dt_sec;
    float D = loop.pid.kd * derivative;

    // Save error for next derivative calculation
    loop.state.prev_error = error;

    return P + I + D;
}

/* Helper functions for limiting values */

float PID::limit_range(float value, float min_value, float max_value)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }

    return value;
}

uint8_t PID::clamp_duty_cycle(float duty_cycle)
{
    if (duty_cycle <= 0.0f)
    {
        return 0;
    }
    if (duty_cycle >= 100.0f)
    {
        return 100;
    }

    return static_cast<uint8_t>(duty_cycle);
}

bool PID::ticks_to_velocity(const EncoderInput& encoder, float& left_velocity,
                            float& right_velocity)
{
    static int32_t prev_l_ticks = 0;
    static int32_t prev_r_ticks = 0;

    float dt_sec = Elapsed::get_dt_sec();

    int32_t delta_left_ticks = encoder.left_ticks - prev_l_ticks;
    int32_t delta_right_ticks = encoder.right_ticks - prev_r_ticks;

    prev_l_ticks = encoder.left_ticks;
    prev_r_ticks = encoder.right_ticks;

    left_velocity =
        (delta_left_ticks / static_cast<float>(kTicksPerRevolution)) *
        kWheelCircumference / dt_sec;
    right_velocity =
        (delta_right_ticks / static_cast<float>(kTicksPerRevolution)) *
        kWheelCircumference / dt_sec;

    return true;
}

}  // namespace MM