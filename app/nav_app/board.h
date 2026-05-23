/**
 * @file board.h
 * @brief Navigation app board interface.
 * @author Bex Saw
 */

#pragma once

#include "ircontroller.h"
#include "motioncontroller.h"

namespace MM
{

struct Board
{
    IrController& ir_controller;
    MotionController& motion_controller;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM
