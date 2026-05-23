/**
 * @file board.h
 * @brief CLI app board interface — exposes all hardware peripherals.
 * @author Bex Saw
 */

#pragma once

#include "drv8231.h"
#include "encoder.h"
#include "gpio.h"
#include "ircontroller.h"
#include "motioncontroller.h"

namespace MM
{

struct Board
{
    // Motion hardware
    MotionController& motion_controller;
    IrController& ir_controller;

    // UI hardware
    Gpio& led1;
    Gpio& led2;
    Gpio& led3;
    Gpio& search_bt;
    Gpio& zoom_bt;
};

bool board_init(void);
Board& get_board(void);

}  // namespace MM
