#pragma once

#include "adc.h"
#include "dma.h"

namespace MM
{
struct Board
{
    Adc& adc;
    Dma& dma;
};

bool board_init();
Board& get_board();
};  // namespace MM