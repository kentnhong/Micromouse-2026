/**
* @file board.h
* @author Bex Saw
* @brief Board specific definitions for Bluetooth USART test on F411
*/

#pragma once
#include "gpio.h"
#include "usart.h"

extern uint8_t rx_byte;
namespace MM
{
struct Board
{
    Usart& usart;
    Gpio& rx;
    Gpio& tx;
    //Gpio& key;  // KEY pin for controlling HC-05 mode
};

bool bsp_init();
Board& get_board();

}  // namespace MM
