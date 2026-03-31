#include "pid.h"
#include <cmath>
<<<<<<< HEAD
<<<<<<< HEAD
#include <numbers>
=======
>>>>>>> 0346965 (Fix Yaw back for abosulate heading)
=======
#include <numbers>
>>>>>>> 122cb97 (output to pwm duty cycle)

namespace MM
{

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 31fdaba (output to duty cycle)
static constexpr float kMaxAngle = 180.0f;
<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)
static constexpr float kWheelCircumference =
    std::numbers::pi * 0.0381f;  // 1.5 inch diameter wheel => C = pi * d
static constexpr float kTicksPerRevolution =
    12 * 4;  // 12 ticks per revolution from encoder, x4 for quadrature encoding
<<<<<<< HEAD
<<<<<<< HEAD
=======
static constexpr float kWheelCircumference = std::numbers::pi * 0.0381f;  // 1.5 inch diameter wheel => C = pi * d
static constexpr float kTicksPerRevolution = 12 * 4;  // 12 ticks per revolution from encoder, x4 for quadrature encoding
>>>>>>> b74cd35 (notes on PID val)
=======
>>>>>>> 662638f (add elapsed time for ensure the PID controller updates are based on accurate)

static inline float quaternion_to_yaw(const Bno055Data& imu_data)
=======
=======
>>>>>>> 4895d09 (PID structure + ideas)
<<<<<<< HEAD
=======
>>>>>>> f506e16 (PID functions + expected math)
PID::PID(const Val& speed, const Val& turn)
>>>>>>> efd260e (PID control adding init)
{
<<<<<<< HEAD
    // Convert quaternion to yaw angle (in degrees)
    float q0 = imu_data.quat.w;
    float q1 = imu_data.quat.x;
    float q2 = imu_data.quat.y;
    float q3 = imu_data.quat.z;

    // Yaw calculation (We want absolute yaw, not angular velocity)
    float yaw_rad = std::atan2(2.0f * (q0 * q3 + q1 * q2),
                               1.0f - 2.0f * (q2 * q2 + q3 * q3));

    return yaw_rad * (kMaxAngle / std::numbers::pi);  // Convert to degrees
=======
    // Init PID constants for speed & turn loops
    this->speed.pid = speed;
    this->turn.pid = turn;
    this->speed.state = State{};
    this->turn.state = State{};
>>>>>>> 43d9502 (PID math implemented; still need to verify)
}

PID::PID(const PIDConfig& config)
{
<<<<<<< HEAD
<<<<<<< HEAD
    this->left.pid = config.left;
    this->right.pid = config.right;
    this->yaw.pid = config.yaw;
    this->left.state = Context{};
    this->right.state = Context{};
    this->yaw.state = Context{};
}

bool PID::update(const Input& input, const Target& target, float dt_sec,
                 MotorOutput& output)
{
    // Convert encoder ticks to velocity (m/s)
    float left_velocity = 0.0f;
    float right_velocity = 0.0f;
    ticks_to_velocity(input.encoder, left_velocity, right_velocity);

    // Per-wheel PID: calculate error for each wheel
    float left_error = target.left_speed - left_velocity;
    float right_error = target.right_speed - right_velocity;

    // Yaw PID: calculate yaw error (target - measured)
    float yaw_error = limit_angle(target.yaw - quaternion_to_yaw(input.imu));
    float yaw_output = compute_pid(this->yaw, yaw_error, dt_sec);

    // Compute PID outputs for each wheel
    float left_output = compute_pid(this->left, left_error, dt_sec);
    float right_output = compute_pid(this->right, right_error, dt_sec);

    // Mix yaw correction into wheel outputs
    output.left = left_output - yaw_output;
    output.right = right_output + yaw_output;

    return true;
}

float PID::limit_angle(float angle)
{
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
=======
    // TODO: Implement PID update logic
    // Update because we need to compute the control signal based on the current sensor readings and target values
    return true;  
>>>>>>> 2e41c8d (commit for rebase)
=======
    // 1. Calculate target - current error for speed and yaw
    float speed_error = target.speed - input.encoder.speed;
    float yaw_error = limit_angle(target.yaw - input.imu.gyro);
=======
PID::PID(const Val& left, const Val& right)
=======
static constexpr float kPi = 3.14159265f;
=======
>>>>>>> 122cb97 (output to pwm duty cycle)
static constexpr float kMaxAngle = 180.0f;  
=======
>>>>>>> 31fdaba (output to duty cycle)

static inline float quaternion_to_yaw(const Bno055Data& imu_data)
>>>>>>> 0346965 (Fix Yaw back for abosulate heading)
{
    // Convert quaternion to yaw angle (in degrees)
    float q0 = imu_data.quat.w;
    float q1 = imu_data.quat.x;
    float q2 = imu_data.quat.y;
    float q3 = imu_data.quat.z;

    // Yaw calculation (We want absolute yaw, not angular velocity)
    float yaw_rad = std::atan2(2.0f * (q0 * q3 + q1 * q2),
                               1.0f - 2.0f * (q2 * q2 + q3 * q3));

    return yaw_rad * (kMaxAngle / std::numbers::pi);  // Convert to degrees
}

PID::PID(const PIDConfig& config)
{
    this->left.pid = config.left;
    this->right.pid = config.right;
    this->yaw.pid = config.yaw;
    this->left.state = Context{};
    this->right.state = Context{};
    this->yaw.state = Context{};
}

bool PID::update(const Input& input, const Target& target, float dt_sec,
                 MotorOutput& output)
{
    // Convert encoder ticks to velocity (m/s)
    float left_velocity = 0.0f;
    float right_velocity = 0.0f;
    ticks_to_velocity(input.encoder, left_velocity, right_velocity);

    // Per-wheel PID: calculate error for each wheel
<<<<<<< HEAD
    float left_error = target.left_speed - input.encoder.left_velocity;
    float right_error = target.right_speed - input.encoder.right_velocity;
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
=======
    float left_error = target.left_speed - left_velocity;
    float right_error = target.right_speed - right_velocity;
>>>>>>> 31fdaba (output to duty cycle)

    // Yaw PID: calculate yaw error (target - measured)
    float yaw_error = limit_angle(target.yaw - quaternion_to_yaw(input.imu));
    float yaw_output = compute_pid(this->yaw, yaw_error, dt_sec);

    // Compute PID outputs for each wheel
    float left_output = compute_pid(this->left, left_error, dt_sec);
    float right_output = compute_pid(this->right, right_error, dt_sec);

    // Mix yaw correction into wheel outputs
    output.left = left_output - yaw_output;
    output.right = right_output + yaw_output;

    return true;
>>>>>>> 43d9502 (PID math implemented; still need to verify)
}

float PID::limit_angle(float angle)
{
    while (angle > 180.0f) angle -= 360.0f;
    while (angle < -180.0f) angle += 360.0f;
    return angle;
}

bool PID::reset()
{
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    this->left.state = Context{};
    this->right.state = Context{};
    this->yaw.state = Context{};
=======
    this->speed.state = State{};
    this->turn.state = State{};

>>>>>>> 43d9502 (PID math implemented; still need to verify)
=======
    this->left.state = State{};
    this->right.state = State{};
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
=======
    this->left.state = Context{};
    this->right.state = Context{};
    this->yaw.state = Context{};
>>>>>>> 0346965 (Fix Yaw back for abosulate heading)
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

<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
bool PID::output_to_duty_cycle(const MotorOutput& output, float& left_duty,
                               float& right_duty)
{
<<<<<<< HEAD
=======
bool PID::output_to_duty_cycle(const MotorOutput& output, float& left_duty, float& right_duty)
=======
bool PID::output_to_duty_cycle(const MotorOutput& output, float& left_duty,
                               float& right_duty)
>>>>>>> 31fdaba (output to duty cycle)
{
>>>>>>> 122cb97 (output to pwm duty cycle)
    // Convert PID output to duty cycle percentage
    left_duty = limit_range(output.left, min_output, max_output);
    right_duty = limit_range(output.right, min_output, max_output);

    return true;
}

<<<<<<< HEAD
<<<<<<< HEAD
=======
>>>>>>> 983c61d (pid bsp + main.cc)
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

<<<<<<< HEAD
=======
>>>>>>> 122cb97 (output to pwm duty cycle)
=======
>>>>>>> 983c61d (pid bsp + main.cc)
float PID::compute_pid(Loop& loop, float error, float dt_sec)
{

    // Proportional
    // P = p * error
    float P = loop.pid.p * error;

    // Integral
    // I = i * integral(error * dt)
    loop.state.integral += error * dt_sec;
    loop.state.integral =
        limit_range(loop.state.integral, -integral_limit, integral_limit);
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

float PID::limit_range(float value, float min_value, float max_value)
=======
    
=======
float PID::compute_pid(Loop& loop, float error, float dt_sec)
{

>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
    // Proportional
    // P = p * error
    float P = loop.pid.p * error;

    // Integral
    // I = i * integral(error * dt)
    loop.state.integral += error * dt_sec;
    loop.state.integral =
        limit_range(loop.state.integral, -integral_limit, integral_limit);
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

<<<<<<< HEAD
float PID::limit_range (float value, float min_value, float max_value)
>>>>>>> 43d9502 (PID math implemented; still need to verify)
=======
float PID::limit_range(float value, float min_value, float max_value)
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
{
    if (value < min_value)
    {
        return min_value;
    }
    if (value > max_value)
    {
        return max_value;
    }

<<<<<<< HEAD
<<<<<<< HEAD
    return value;
=======
    return value;   
>>>>>>> 43d9502 (PID math implemented; still need to verify)
}

uint8_t PID::clamp_duty_cycle(float duty_cycle)
{
<<<<<<< HEAD
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

<<<<<<< HEAD
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
=======
    while (angle > 180.0f)
    {
        angle -= 360.0f;
    }
    while (angle < -180.0f)
    {
        angle += 360.0f;
    }

    return angle;
>>>>>>> 43d9502 (PID math implemented; still need to verify)
=======
    return value;
>>>>>>> 306c6c6 (Change the math to per wheel (l/r); so we don't deal with yaw, this is only speed of each wheel dictate the direction)
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
=======
}  // namespace MM
<<<<<<< HEAD
=======
PID::PID(float kp, float ki, float kd){}
=======
static constexpr kMinOutput = -100.0f;  // Replace with actual value later
static constexpr kMaxOutput = 100.0f;   // Same as above
>>>>>>> 287ef60 (PID structure + ideas)

PID::PID(Bno055Data& imu_data, EncoderInput& enc_data, Val& pid)
{
}

<<<<<<< HEAD
}
>>>>>>> 4bebdbe (PID control adding init)
<<<<<<< HEAD
>>>>>>> efd260e (PID control adding init)
=======
=======
bool PID::update(const Input& input, const Target& target, Output& output)
{
}

bool PID::reset()
{
}

bool PID::set_output_limits(float min_output, float max_output)
{
    min_output = kMinOutput;
    max_output = kMaxOutput;
}

}  // namespace MM
>>>>>>> 287ef60 (PID structure + ideas)
>>>>>>> 4895d09 (PID structure + ideas)
=======
>>>>>>> f506e16 (PID functions + expected math)
