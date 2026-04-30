/**
 * @file board.h
 * @author Kent Hong
 * @brief BSP interface for timebase testing.
 */

#pragma once
#include "gpio.h"
#include "timebase.h"

namespace MM
{
struct Board
{
    Gpio& pin;
    Timebase& counter;
};

bool board_init();
Board& get_board();
};  // namespace MM