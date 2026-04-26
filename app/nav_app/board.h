/**
 * @file board.h
 * @brief Navigation test board interface
 * @author Bex Saw
 * @date 4/8/2026
 */

#pragma once

#include "bno055_imu.h"
#include "delay.h"
#include "gpio.h"
#include "i2c.h"
#include "nav.h"

namespace MM
{

struct Board
{
    Bno055& imu;
    // TODO: IR sensors, motors, encoders, Gpio, etc.
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM