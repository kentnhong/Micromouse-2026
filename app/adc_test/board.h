#pragma once

#include "adc.h"

namespace MM
{
struct Board
{
    Adc& adc;
};

bool board_init();
Board& get_board();
};  // namespace MM