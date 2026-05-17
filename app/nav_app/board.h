/**
 * @file board.h
 * @brief Navigation test board interface
 * @author Bex Saw
 * @date 4/8/2026
 */

#pragma once

#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "ircontroller.h"
#include "nav.h"

namespace MM
{

struct Board
{
    IrController& ir_controller;
};

bool bsp_init(void);
Board& get_board(void);
}  // namespace MM