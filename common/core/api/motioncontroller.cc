#include "motioncontroller.h"

#include <cstdint>

static constexpr float kPi = 3.14159265358979f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

// Robot geometry — cell = 180 mm unit square; travel 150 mm center-to-center (tuned)
static constexpr float kCellDistanceMm = 47.5f;
static constexpr float kWheelbaseMm = 57.5f;

// Tick distances for each move type
static constexpr int32_t kCellTicks =
    static_cast<int32_t>(kCellDistanceMm / kMmPerTick);
static constexpr int32_t k90DegTurnTicks =
    static_cast<int32_t>((kWheelbaseMm * kPi / 8.0f) / kMmPerTick);
static constexpr int32_t k180DegTurnTicks = k90DegTurnTicks * 2;

// Straight profile — moderate speed, gentle ramps for reliable cell stopping
static constexpr float kStraightMinTicks = 150.0f;     // ticks/s
static constexpr float kStraightMaxTicks = 400.0f;     // ticks/s
static constexpr float kStraightAccelTicks = 600.0f;   // ticks/s²
static constexpr float kStraightDecelTicks = 1200.0f;  // ticks/s²

// Turn profile — very slow for body clearance during pivot
static constexpr float kTurnMinTicks = 150.0f;    // ticks/s
static constexpr float kTurnMaxTicks = 250.0f;    // ticks/s
static constexpr float kTurnAccelTicks = 350.0f;  // ticks/s²
static constexpr float kTurnDecelTicks = 350.0f;  // ticks/s²

// Wall centering: P corrects drift, small D damps overshoot
static constexpr uint16_t kWallSideThreshold = 3667;
static constexpr float kWallCenterKp = 0.10f;
static constexpr float kWallCenterKd = 0.003f;

// Front wall: stop when robot is very close — must be well above normal open-corridor reading (~3846)
static constexpr uint16_t kFrontWallThreshold = 4000;

namespace MM
{

MotionController::MotionController(MotionControllerParams p)
    : motor_left(p.motor_left),
      motor_right(p.motor_right),
      encoder_left(p.encoder_left),
      encoder_right(p.encoder_right),
      pid_left(p.motor_left, p.gains),
      pid_right(p.motor_right, p.gains),
      encoder_timing(
          Sample::init_encoder_timing(p.encoder_left, p.encoder_sample_us)),
      encoder_sample_us(p.encoder_sample_us),
      right_speed_scale(p.right_speed_scale)
{
    pid_left.set_output_limits(0.0f, p.max_output_left);
    pid_right.set_output_limits(0.0f, p.max_output_right);
}

void MotionController::init()
{
    encoder_left.init_cycle_counter();
    encoder_timing =
        Sample::init_encoder_timing(encoder_left, encoder_sample_us);
}

bool MotionController::forward(const IrValues& ir)
{
    if (move_state != MoveState::FORWARD)
    {
        const int32_t total_ticks = kCellTicks * straight_cells_remaining;
        profile.configure(kStraightMinTicks, kStraightMaxTicks,
                          kStraightAccelTicks, kStraightDecelTicks,
                          total_ticks);
        pid_left.reset();
        pid_right.reset();
        ir_front_left_smooth = 0.0f;
        ir_front_right_smooth = 0.0f;
        ir_wall_error_prev = 0.0f;
        ir_left_was_wall = false;
        ir_right_was_wall = false;
        move_state = MoveState::FORWARD;
    }

    const EncoderInput enc =
        Sample::sample_encoders(encoder_left, encoder_right, encoder_timing);
    const float dt = encoder_timing.sample_time_sec;

    profile.trapezoidal(enc, dt);
    const float base_speed = profile.get_speed_setpoint();

    // Exponential low-pass on raw IR (α=0.4)
    ir_left_smooth = 0.4f * static_cast<float>(ir.left) + 0.6f * ir_left_smooth;
    ir_right_smooth =
        0.4f * static_cast<float>(ir.right) + 0.6f * ir_right_smooth;
    ir_front_left_smooth =
        0.4f * static_cast<float>(ir.front_left) + 0.6f * ir_front_left_smooth;
    ir_front_right_smooth = 0.4f * static_cast<float>(ir.front_right) +
                            0.6f * ir_front_right_smooth;

    // Front wall: stop immediately if either front sensor sees a wall
    if (ir_front_left_smooth > kFrontWallThreshold ||
        ir_front_right_smooth > kFrontWallThreshold)
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right,
                                    encoder_timing);
        }
        move_state = MoveState::IDLE;
        return true;
    }

    // Wall-edge detection: a wall that was present just disappeared → junction reached
    const bool left_wall = ir_left_smooth > kWallSideThreshold;
    const bool right_wall = ir_right_smooth > kWallSideThreshold;
    if ((ir_left_was_wall && !left_wall) || (ir_right_was_wall && !right_wall))
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right,
                                    encoder_timing);
        }
        ir_left_was_wall = false;
        ir_right_was_wall = false;
        move_state = MoveState::IDLE;
        return true;
    }
    ir_left_was_wall = left_wall;
    ir_right_was_wall = right_wall;

    // Wall centering PD: use both walls when available, fall back to single wall
    float correction = 0.0f;
    if (left_wall || right_wall)
    {
        float wall_error = 0.0f;
        if (left_wall && right_wall)
            wall_error = ir_right_smooth - ir_left_smooth;
        else if (right_wall)
            wall_error =
                ir_right_smooth - kWallSideThreshold;  // too close to right
        else
            wall_error =
                -(ir_left_smooth - kWallSideThreshold);  // too close to left
        const float d_error = (wall_error - ir_wall_error_prev) / dt;
        correction = kWallCenterKp * wall_error + kWallCenterKd * d_error;
        ir_wall_error_prev = wall_error;
    }

    pid_left.update(base_speed - correction, Drv8231::Direction::FORWARD,
                    enc.left_ticks, dt);
    pid_right.update((base_speed + correction) * right_speed_scale,
                     Drv8231::Direction::FORWARD, enc.right_ticks, dt);

    if (profile.is_complete())
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right,
                                    encoder_timing);
        }
        move_state = MoveState::IDLE;
        return true;
    }
    return false;
}

void MotionController::set_straight_cells(int cells)
{
    straight_cells_remaining = cells > 0 ? cells : 1;
}

// Shared pivot-turn logic: right motor FW + left motor RV (positive distance = left turn)
// For right turn, caller passes TURNING_RIGHT and the motors are swapped inside.
bool MotionController::run_turn(MoveState turn_state, int32_t distance_ticks)
{
    if (move_state != turn_state)
    {
        profile.configure(kTurnMinTicks, kTurnMaxTicks, kTurnAccelTicks,
                          kTurnDecelTicks, distance_ticks);
        pid_left.reset();
        pid_right.reset();
        move_state = turn_state;
    }

    const EncoderInput enc =
        Sample::sample_encoders(encoder_left, encoder_right, encoder_timing);
    const float dt = encoder_timing.sample_time_sec;

    // For left turn: left motor reverses (negative ticks), right motor forwards (positive)
    // For right turn: left motor forwards (positive), right motor reverses (negative)
    int32_t fwd_ticks = 0;
    int32_t rev_ticks = 0;

    if (turn_state == MoveState::TURNING_LEFT ||
        turn_state == MoveState::U_TURN)
    {
        fwd_ticks = enc.right_ticks;  // right goes forward
        rev_ticks =
            -enc.left_ticks;  // left goes backward (negate to get positive value)
    }
    else  // TURNING_RIGHT
    {
        fwd_ticks = enc.left_ticks;    // left goes forward
        rev_ticks = -enc.right_ticks;  // right goes backward
    }

    // Use average of both wheels for profile progress
    profile.trapezoidal(EncoderInput{rev_ticks, fwd_ticks}, dt);
    const float speed = profile.get_speed_setpoint();

    const float right_speed = speed * right_speed_scale;
    if (turn_state == MoveState::TURNING_LEFT ||
        turn_state == MoveState::U_TURN)
    {
        pid_left.update(speed, Drv8231::Direction::REVERSE, rev_ticks, dt);
        pid_right.update(right_speed, Drv8231::Direction::FORWARD, fwd_ticks,
                         dt);
    }
    else
    {
        pid_left.update(speed, Drv8231::Direction::FORWARD, fwd_ticks, dt);
        pid_right.update(right_speed, Drv8231::Direction::REVERSE, rev_ticks,
                         dt);
    }

    if (profile.is_complete())
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right,
                                    encoder_timing);
        }
        move_state = MoveState::IDLE;
        return true;
    }
    return false;
}

bool MotionController::turn_left()
{
    return run_turn(MoveState::TURNING_LEFT, k90DegTurnTicks);
}

bool MotionController::turn_right()
{
    return run_turn(MoveState::TURNING_RIGHT, k90DegTurnTicks);
}

bool MotionController::u_turn()
{
    return run_turn(MoveState::U_TURN, k180DegTurnTicks);
}

void MotionController::stop()
{
    motor_left.drive(Drv8231::Direction::COAST, 0);
    motor_right.drive(Drv8231::Direction::COAST, 0);
    move_state = MoveState::IDLE;
}

}  // namespace MM
