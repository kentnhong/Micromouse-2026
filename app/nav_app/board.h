/**
 * @file board.h
 * @brief Navigation test board interface
 * @author Bex Saw
 * @date 4/8/2026
 */

#pragma once

#include "delay.h"
#include "gpio.h"
#include "ircontroller.h"
#include "nav.h"

namespace MM
{

struct Board
{
<<<<<<< HEAD
    IrController& ir_controller;
=======
    Bno055& imu;
    // TODO: IR sensors, motors, encoders, Gpio, etc.
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
};

bool bsp_init(void);
Board& get_board(void);
}  // namespace MM