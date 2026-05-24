#include "motioncontroller.h"

#include <cstdint>

static constexpr float kPi = 3.14159265358979f;
static constexpr float kWheelDiameterMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerMotorRev = 8.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

// Robot geometry
static constexpr float kCellDistanceMm = 155.0f;
static constexpr float kWheelbaseMm = 73.0f;  // center-to-center track width, tune this

// Tick distances for each move type
static constexpr int32_t kCellTicks =
    static_cast<int32_t>(kCellDistanceMm / kMmPerTick);
static constexpr int32_t k90DegTurnTicks =
    static_cast<int32_t>((kWheelbaseMm * kPi / 4.0f) / kMmPerTick);
static constexpr int32_t k180DegTurnTicks =
    static_cast<int32_t>((kWheelbaseMm * kPi / 2.0f) / kMmPerTick);

// Straight drive profile (mm/s and mm/s^2, converted to ticks)
static constexpr float kStraightMinMmPerSec = 50.0f;
static constexpr float kStraightMaxMmPerSec = 190.0f;
static constexpr float kStraightAccelMmPerSec2 = 500.0f;
static constexpr float kStraightDecelMmPerSec2 = 500.0f;
static constexpr float kStraightMinTicks = kStraightMinMmPerSec / kMmPerTick;
static constexpr float kStraightMaxTicks = kStraightMaxMmPerSec / kMmPerTick;
static constexpr float kStraightAccelTicks = kStraightAccelMmPerSec2 / kMmPerTick;
static constexpr float kStraightDecelTicks = kStraightDecelMmPerSec2 / kMmPerTick;

// Turn profile (slower for accuracy)
static constexpr float kTurnMinMmPerSec = 40.0f;
static constexpr float kTurnMaxMmPerSec = 120.0f;
static constexpr float kTurnAccelMmPerSec2 = 400.0f;
static constexpr float kTurnDecelMmPerSec2 = 400.0f;
static constexpr float kTurnMinTicks = kTurnMinMmPerSec / kMmPerTick;
static constexpr float kTurnMaxTicks = kTurnMaxMmPerSec / kMmPerTick;
static constexpr float kTurnAccelTicks = kTurnAccelMmPerSec2 / kMmPerTick;
static constexpr float kTurnDecelTicks = kTurnDecelMmPerSec2 / kMmPerTick;

// Wall centering: correction in ticks/s per raw IR unit of imbalance
static constexpr uint16_t kWallSideThreshold = 2000;
static constexpr float kWallCenterGain = 0.005f;

namespace MM
{

MotionController::MotionController(MotionControllerParams p)
    : motor_left(p.motor_left),
      motor_right(p.motor_right),
      encoder_left(p.encoder_left),
      encoder_right(p.encoder_right),
      pid_left(p.motor_left, p.gains),
      pid_right(p.motor_right, p.gains),
      encoder_timing(Sample::init_encoder_timing(p.encoder_left, p.encoder_sample_us))
{
    pid_left.set_output_limits(-1.0f, 0.80f);
    pid_right.set_output_limits(-1.0f, 0.80f);
}

bool MotionController::forward(const IrValues& ir)
{
    if (move_state != MoveState::FORWARD)
    {
        const int32_t total_ticks = kCellTicks * straight_cells_remaining;
        profile.configure(kStraightMinTicks, kStraightMaxTicks,
                          kStraightAccelTicks, kStraightDecelTicks, total_ticks);
        pid_left.reset();
        pid_right.reset();
        move_state = MoveState::FORWARD;
    }

    const EncoderInput enc =
        Sample::sample_encoders(encoder_left, encoder_right, encoder_timing);
    const float dt = encoder_timing.sample_time_sec;

    profile.trapezoidal(enc, dt);
    const float base_speed = profile.get_speed_setpoint();

    // Wall centering: steer away from the closer wall when walls exist on both sides
    float correction = 0.0f;
    if (ir.left > kWallSideThreshold && ir.right > kWallSideThreshold)
    {
        correction = (static_cast<float>(ir.right) - static_cast<float>(ir.left)) *
                     kWallCenterGain;
    }

    pid_left.update(base_speed - correction, Drv8231::Direction::FORWARD,
                    enc.left_ticks, dt);
    pid_right.update(base_speed + correction, Drv8231::Direction::FORWARD,
                     enc.right_ticks, dt);

    if (profile.is_complete())
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right, encoder_timing);
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

    if (turn_state == MoveState::TURNING_LEFT || turn_state == MoveState::U_TURN)
    {
        fwd_ticks = enc.right_ticks;   // right goes forward
        rev_ticks = -enc.left_ticks;   // left goes backward (negate to get positive value)
    }
    else  // TURNING_RIGHT
    {
        fwd_ticks = enc.left_ticks;    // left goes forward
        rev_ticks = -enc.right_ticks;  // right goes backward
    }

    // Use average of both wheels for profile progress
    profile.trapezoidal(EncoderInput{rev_ticks, fwd_ticks}, dt);
    const float speed = profile.get_speed_setpoint();

    if (turn_state == MoveState::TURNING_LEFT || turn_state == MoveState::U_TURN)
    {
        pid_left.update(speed, Drv8231::Direction::REVERSE, rev_ticks, dt);
        pid_right.update(speed, Drv8231::Direction::FORWARD, fwd_ticks, dt);
    }
    else
    {
        pid_left.update(speed, Drv8231::Direction::FORWARD, fwd_ticks, dt);
        pid_right.update(speed, Drv8231::Direction::REVERSE, rev_ticks, dt);
    }

    if (profile.is_complete())
    {
        motor_left.drive(Drv8231::Direction::COAST, 0);
        motor_right.drive(Drv8231::Direction::COAST, 0);
        for (int i = 0; i < 5; ++i)
        {
            Sample::sample_encoders(encoder_left, encoder_right, encoder_timing);
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
