#include "pid.h"
#include <numbers>

namespace MM
{

static constexpr float kWheelDiameterM = 0.014f;  // 14mm diameter wheel
static constexpr float kGearRatio = 15.25f;
static constexpr float kTicksPerRev = 12.0f;

static constexpr float kTicksPerOutputRev =
    kGearRatio * kTicksPerRev;  // 180 ticks/output rev
static constexpr float kWheelCircumferenceM =
    kWheelDiameterM *
    std::numbers::pi_v<float>;  // 0.044 m or 4.4 cm circumference
static constexpr float kMetersPerTick =
    kWheelCircumferenceM /
    kTicksPerOutputRev;  // 0.000244 m/tick or 0.0244 cm/tick

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
    if (dt_sec <= 0.0f)
    {
        output.left = 0.0f;
        output.right = 0.0f;
        return false;
    }

    // Convert encoder ticks to velocity (m/s)
    float left_velocity = 0.0f;
    float right_velocity = 0.0f;
    ticks_to_velocity(encoder, dt_sec, left_velocity, right_velocity);

    // Per-wheel PID: calculate error for each wheel (no yaw)
    float left_error = target.left_speed - left_velocity;
    float right_error = target.right_speed - right_velocity;

    float left_output = compute_pid(this->left, left_error, dt_sec);
    float right_output = compute_pid(this->right, right_error, dt_sec);

    output.left = left_output;
    output.right = right_output;

    return true;
}

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

    return limit_range(P + I + D, min_output, max_output);
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

bool PID::ticks_to_velocity(const EncoderInput& encoder, float dt_sec,
                            float& left_velocity, float& right_velocity)
{
    if (dt_sec <= 0.0f)
    {
        left_velocity = 0.0f;
        right_velocity = 0.0f;
        return false;
    }

    left_velocity = (encoder.left_ticks * kMetersPerTick) / dt_sec;
    right_velocity = (encoder.right_ticks * kMetersPerTick) / dt_sec;

    return true;
}

bool PID::set_target_speed(float left_speed, float right_speed, Target& target)
{
    target.left_speed = left_speed;
    target.right_speed = right_speed;

    return true;
}

}  // namespace MM
