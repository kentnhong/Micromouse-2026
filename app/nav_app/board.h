/**
 * @file board.h
 * @brief Navigation app board interface.
 * @author Bex Saw
 */

#pragma once

#include "gpio.h"
#include "ircontroller.h"
#include "motioncontroller.h"

namespace MM
{

struct Board
{
    IrController& ir_controller;
    MotionController& motion_controller;
    Gpio& start_bt;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM
