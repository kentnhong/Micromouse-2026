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
#include "pwm.h"

namespace MM
{

struct Board
{
    // ENCODER
    Encoder& encoder;

    // PWM
    Pwm& pwm1;
    Pwm& pwm2;

    // MOTOR
    Drv8231& motor;

    // GPIO
    Gpio& in1;
    Gpio& in2;
    Gpio& encoder_ch1;
    Gpio& encoder_ch2;

    uint32_t encoder_sample_us;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM