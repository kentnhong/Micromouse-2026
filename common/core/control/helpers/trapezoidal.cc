#include "trapezoidal.h"

namespace MM
{

Trapezoidal::VelocitySetpoint Trapezoidal::trapezoidal(
    float target, const MM::EncoderInput& encoder, float ticks_to_mm,
    uint8_t turn)
{
    // setpoint l/r initialize at 0
    VelocitySetpoint setpoint{0.0f, 0.0f};

    // turn muliplier is -1 for turn, 1 for straight
    float turnMul = (turn) ? -1.0f : 1.0f;

    // convert encoder ticks to distance traveled in mm
    float left_travel = encoder.left_ticks * ticks_to_mm;
    float right_travel = encoder.right_ticks * ticks_to_mm;

    // absolute target distance for easier calculations (direction handled by setpoint sign)
    float abs_target = (target < 0) ? -target : target;

    // Adjust acceleration distance if the target is too short to reach max speed
    float acceleration_dist = acceleration_distance;
    if (0.5f * abs_target < acceleration_dist)
    {
        acceleration_dist = 0.5f * abs_target;
    }

    // calculate setpoint based on current travel and target using a lambda for cleaner code
    static inline auto calc_setpoint = [&](float travel) -> float
    {
        float abs_travel = std::abs(travel);
        float velocity;

        // If we've overshot the target, set velocity to minimum in the opposite direction
        if (abs_travel > abs_target + target_distance_threshold)
        {
            return -min_speed;
        }

        // If we're within the target threshold, stop
        if (abs_travel >= abs_target - target_distance_threshold)
        {
            return 0.0f;
        }

        // Acceleration phase
        if (abs_travel <= acceleration_dist)
        {
            velocity = abs_travel / acceleration_distance * max_speed;
        }
        // Deceleration phase
        else if (abs_target - abs_travel <= acceleration_dist)
        {
            velocity =
                (abs_target - abs_travel) / acceleration_distance * max_speed;
        }
        else
        {
            // Constant speed phase
            velocity = max_speed;
        }
        return (velocity < min_speed) ? min_speed : velocity;
    };

    // Calculate setpoints for left and right wheels, applying turn multiplier to left wheel
    setpoint.left = calc_setpoint(left_travel) * turnMul;
    setpoint.right = calc_setpoint(right_travel);

    return setpoint;
}

}  // namespace MM