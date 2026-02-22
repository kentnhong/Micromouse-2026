/**
* @file board.h
* @author Bex Saw
* @brief Board specific definitions for USART test on F411
*/

#pragma once
#include "gpio.h"
#include "uart.h"

namespace MM
{
struct Board
{
    Usart& usart;
    Gpio& rx;
    Gpio& tx;
};

bool bsp_init();
Board& get_board();

}  // namespace MM
