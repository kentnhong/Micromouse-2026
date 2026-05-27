/**
* @file motioncontroller.h
* @brief High-level motion controller for Micromouse FW/RV/L/R movement.
* @author Bex Saw
* @date 5/17/2026
*/

#pragma once

#include "drv8231.h"
#include "enc_sample.h"
#include "encoder.h"
#include "ircontroller.h"
#include "pid.h"
#include "pid_math.h"
#include "trapezoidal.h"

namespace MM
{

struct MotionControllerParams
{
    Drv8231& motor_left;
    Drv8231& motor_right;
    Encoder& encoder_left;
    Encoder& encoder_right;
    Gains gains;
    uint32_t encoder_sample_us;
    float max_output_left = 1.0f;
    float max_output_right = 1.0f;
    float right_speed_scale =
        1.0f;  // < 1.0 if right motor runs faster than left
};

class MotionController
{
public:
    explicit MotionController(MotionControllerParams params);

    /**
    * @brief Re-initialise encoder timing with the correct CPU clock.
    * Must be called from bsp_init() after the PLL is running, because the
    * constructor runs before main() when SystemCoreClock is still HSI (16 MHz).
    */
    void init();

    /**
    * @brief Drive straight for one cell (or straight_cells_remaining cells in zoom mode).
    * @return true when the cell(s) are complete and nav should make the next decision.
    */
    bool forward(const IrValues& ir);

    /**
    * @brief Tell the controller how many straight cells remain (for trapezoidal decel).
    */
    void set_straight_cells(int cells);

    /**
    * @brief Turn 90 degrees left (right motor FW, left motor RV).
    * @return true when the turn is complete.
    */
    bool turn_left();

    /**
    * @brief Turn 90 degrees right (left motor FW, right motor RV).
    * @return true when the turn is complete.
    */
    bool turn_right();

    /**
    * @brief Turn 180 degrees (right motor FW, left motor RV).
    * @return true when the turn is complete.
    */
    bool u_turn();

    /**
    * @brief Brake both motors and reset internal move state.
    */
    void stop();

private:
    enum class MoveState : uint8_t
    {
        IDLE,
        FORWARD,
        TURNING_LEFT,
        TURNING_RIGHT,
        U_TURN,
    };

    bool run_turn(MoveState turn_state, int32_t distance_ticks);

    Drv8231& motor_left;
    Drv8231& motor_right;
    Encoder& encoder_left;
    Encoder& encoder_right;
    PID pid_left;
    PID pid_right;
    Trapezoidal profile;
    Sample::EncoderTiming encoder_timing;

    uint32_t encoder_sample_us;
    float right_speed_scale{1.0f};

    MoveState move_state{MoveState::IDLE};
    int straight_cells_remaining{1};

    // Exponential low-pass filter state for IR wall centering (match 2024 α=0.4)
    float ir_left_smooth{0.0f};
    float ir_right_smooth{0.0f};
    float ir_front_left_smooth{0.0f};
    float ir_front_right_smooth{0.0f};
    float ir_wall_error_prev{0.0f};
    bool ir_left_was_wall{false};
    bool ir_right_was_wall{false};
};

}  // namespace MM
