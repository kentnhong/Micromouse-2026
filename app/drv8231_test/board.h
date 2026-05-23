/**
 * @file board.h
 * @brief Drv8231 test board interface
 * @author Bex Saw
 * @date 3/20/2026
 */

#pragma once

#include "delay.h"
#include "drv8231.h"
#include "gpio.h"
#include "pwm.h"

namespace MM
{

struct Board
{
    Drv8231& drv8231_left;
    Drv8231& drv8231_right;
    Pwm& pwm1_left;
    Pwm& pwm2_left;
    Pwm& pwm1_right;
    Pwm& pwm2_right;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM