#pragma once

#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "sys_clk.h"
#include "timebase.h"
#include "usart.h"

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
    Gpio& tx;
    Timebase& delay;
};

bool board_init();
Board& get_board();
};  // namespace MM