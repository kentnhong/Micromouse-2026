/**
* @file api.h
* @brief High-level API for Micromouse motion controller.
* @author Bex Saw
* @date 5/17/2026
*/

#pragma once

#include "drv8231.h"
#include "encoder.h"
#include "ircontroller.h"
// #include "pid.h" TODO: Add PID header when implemented

namespace MM
{
class MotionController
{
public:
    /**
    * @brief Drive straight for one cell, using IR sensor data to maintain alignment.
    */
    bool forward(const IrValues& ir);

    /**
    * @brief Turn the robot 90 degrees to the left.
    * @return true if the turn was successful, false otherwise.
    */
    bool turn_left();

    /**
    * @brief Turn the robot 90 degrees to the right.
    * @return true if the turn was successful, false otherwise.
    */
    bool turn_right();

    /**
    * @brief Perform a 180-degree u-turn.
    * @return true if the u-turn was successful, false otherwise.
    */
    bool u_turn();

    /**
    * @brief Stop all robot motors.
    */
    void stop();

private:
    // PID
    // Encoder
    // Hardware references (e.g., motor drivers)
};
}  // namespace MM