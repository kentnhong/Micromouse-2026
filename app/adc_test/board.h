#pragma once

#include "adc.h"
#include "dma.h"
#include "gpio.h"

namespace MM
{
struct Board
{
    Adc& adc;
    Dma& dma;
    Gpio& ir_led;
};

bool board_init();
Board& get_board();
};  // namespace MM