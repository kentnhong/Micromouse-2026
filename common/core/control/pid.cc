#include "pid.h"

namespace MM
{

PID::PID(const Val& speed, const Val& turn)
{
    // TODO: fetch PID constants from flash and initialize internal state
}

bool PID::update(const Input& input, const Target& target, float dt_sec, MotorOutput& output)
{
    // TODO: Implement PID update logic
    // Update because we need to compute the control signal based on the current sensor readings and target values
    return true;  
}

bool PID::reset()
{
    // TODO: Reset internal state of the PID controller
    return true;
}

void PID::set_output_limits(float min_output, float max_output)
{
    // TODO: Set the output limits for the motor control signals
}

void PID::set_integral_limit(float integral_limit)
{
    // TODO: Set the integral limit to prevent windup
}

float PID::compute_pid (Loop& loop, float error, float dt_sec)
{
    // TODO: Compute the PID control signal based on the error and time step
    return 0.0f;
}

float PID::limit_range (float value, float min_value, float max_value)
{
    if (value < min_value)
        return min_value;
    if (value > max_value)
        return max_value;
    return value;   
}

float PID::limit_angle(float angle)
{
    while (angle > 180.0f)
        angle -= 360.0f;
    while (angle < -180.0f)
        angle += 360.0f;
    return angle;
}

}  // namespace MM