/**
 * @file board.h
 * @brief Trap test board interface — MotionController, USART, start button.
 * @author Bex Saw
 */

#pragma once

#include "gpio.h"
#include "ircontroller.h"
#include "motioncontroller.h"
#include "usart.h"

namespace MM
{

struct Board
{
    MotionController& motion;
    IrController& ir;
    Usart& usart;
    Gpio& start_bt;
};

bool bsp_init();
Board& get_board();

}  // namespace MM
