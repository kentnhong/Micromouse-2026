/**
 * @file board.h
 * @brief Trap test board interface — motors, encoders, USART, start button.
 * @author Bex Saw
 */

#pragma once

#include "drv8231.h"
#include "encoder.h"
#include "gpio.h"
#include "usart.h"

namespace MM
{

struct Board
{
    Drv8231& motor_left;
    Drv8231& motor_right;
    Encoder& encoder_left;
    Encoder& encoder_right;
    Usart&   usart;
    Gpio&    start_bt;
};

bool bsp_init();
Board& get_board();

}  // namespace MM
