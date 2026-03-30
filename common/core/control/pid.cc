#include "pid.h"

namespace MM
{

PID::PID(const Val& speed, const Val& turn)
{
    // Init PID constants for speed & turn loops
    this->speed.pid = speed;
    this->turn.pid = turn;
    this->speed.state = State{};
    this->turn.state = State{};
}

bool PID::update(const Input& input, const Target& target, float dt_sec, MotorOutput& output)
{
    // 1. Calculate target - current error for speed and yaw
    float speed_error = target.speed - input.encoder.speed;
    float yaw_error = limit_angle(target.yaw - input.imu.gyro);

    // 2. Compute PID outputs for speed and yaw
    float speed_output = compute_pid(this->speed, speed_error, dt_sec);
    float yaw_output = compute_pid(this->turn, yaw_error, dt_sec);

    // 3. Set motor outputs (need to convert this values into PWM duty cycle)
    output.left = speed_output - yaw_output;
    output.right = speed_output + yaw_output;

    return true;
}

bool PID::reset()
{
    this->speed.state = State{};
    this->turn.state = State{};

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

float PID::compute_pid (Loop& loop, float error, float dt_sec)
{
    
    // Proportional
    // P = p * error
    float P = loop.pid.p * error;

    // Integral
    // I = i * integral(error * dt)
    loop.state.integral += error * dt_sec;
    loop.state.integral = limit_range(loop.state.integral, -integral_limit, integral_limit);
    float I = loop.pid.i * loop.state.integral;

    // Derivative
    // D = d * (error - prev_error) / dt
    float derivative = (error - loop.state.prev_error) / dt_sec;
    float D = loop.pid.d * derivative;

    // Save error for next derivative calculation
    loop.state.prev_error = error;

    return P + I + D; 
}

/* Helper functions for limiting values */

float PID::limit_range (float value, float min_value, float max_value)
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

float PID::limit_angle(float angle)
{
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }
    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
}

}  // namespace MM