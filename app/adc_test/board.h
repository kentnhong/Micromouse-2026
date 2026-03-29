#pragma once

#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "usart.h"
#include "sys_clk.h"

extern uint8_t rx_byte;

namespace MM
{
struct Board
{
    Adc& adc;
    Dma& dma;
    Gpio& ir_led;
    Usart& usart;
    Clock& clk;
};

bool board_recover();
bool board_init();
Board& get_board();
};  // namespace MM