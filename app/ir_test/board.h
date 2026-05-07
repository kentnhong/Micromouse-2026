/**
 * @file board.h
 * @author Kent Hong
 * @brief BSP Interface for IR Controller testing.
 */

#pragma once
#include "adc.h"
#include "dma.h"
#include "gpio.h"
#include "ircontroller.h"
#include "irsensor.h"
#include "timebase.h"

namespace MM
{
struct Board
{
    IrController& ir_controller;
    Timebase& timebase;
};

bool board_init();
Board& get_board();
}  // namespace MM
