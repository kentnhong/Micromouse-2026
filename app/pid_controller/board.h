/**
 * @file board.h
 * @brief PID controller board interface
 * @author Bex Saw
 * @date 3/31/2026
 */

#pragma once

#include <cstdint>
#include <tuple>

#include "delay.h"
#include "drv8231.h"
#include "enc_sample.h"
#include "encoder.h"
#include "gpio.h"
#include "i2c.h"
#include "pwm.h"

namespace MM
{

/// NOTE: This is currently a setup for one motor and encoder.

struct Board
{
    // Peripherals
    Encoder& encoder;
    Pwm& speed_pwm;
    Drv8231& motor;

    // GPIO pins for motor direction control
    Gpio& in1;
    Gpio& in2;

    // Encoder channel GPIO pins A/B for inputs
    Gpio& encoder_ch1;
    Gpio& encoder_ch2;

    // Need to feed this into the control loop for timing the encoder sampling
    uint32_t encoder_sample_us;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM
