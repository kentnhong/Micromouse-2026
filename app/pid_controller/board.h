/**
 * @file board.h
 * @brief PID controller board interface — dual motor (left / right)
 * @author Bex Saw
 * @date 3/31/2026
 */

#pragma once

#include <cstdint>
#include "delay.h"
#include "drv8231.h"
#include "enc_sample.h"
#include "encoder.h"
#include "gpio.h"
#include "pwm.h"
#include "usart.h"

namespace MM
{

struct Board
{
    // Left motor
    Encoder& encoder_left;
    Pwm& pwm1_left;
    Pwm& pwm2_left;
    Drv8231& motor_left;
    Gpio& in1_left;
    Gpio& in2_left;
    Gpio& enc_left_ch1;
    Gpio& enc_left_ch2;

    // Right motor
    Encoder& encoder_right;
    Pwm& pwm1_right;
    Pwm& pwm2_right;
    Drv8231& motor_right;
    Gpio& in1_right;
    Gpio& in2_right;
    Gpio& enc_right_ch1;
    Gpio& enc_right_ch2;

    Usart& usart;
    uint32_t encoder_sample_us;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM
