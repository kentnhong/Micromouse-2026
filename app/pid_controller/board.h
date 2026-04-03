/**
 * @file board.h
 * @brief PID controller board interface
 * @author Bex Saw
 * @date 3/31/2026
 */

#pragma once

#include <tuple>
#include "bno055_imu.h"
#include "delay.h"
#include "drv8231.h"
#include "elapsed.h"
#include "encoder.h"
#include "gpio.h"
#include "i2c.h"
#include "pid.h"
#include "pwm.h"

namespace MM
{

struct Board
{
    Bno055& imu;
    I2c& i2c;
    Encoder& encoder;
    Pwm& pwm;
    Drv8231& drv8231;
    Gpio& in1;
    Gpio& in2;
};

bool bsp_init(void);
Board& get_board(void);

/* PID */
std::tuple<PID&, PID::Input&, PID::MotorOutput&, PID::Target&> get_pid_bundle();

}  // namespace MM