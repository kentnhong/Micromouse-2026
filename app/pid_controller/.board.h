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
#include "pid.h"
#include "pwm.h"

namespace MM
{

struct Board
{
    // Encoder
    Encoder& left_encoder;
    Encoder& right_encoder;

    // PWM outputs for motor speed control
    Pwm& left_speed_pwm;
    Pwm& right_speed_pwm;

    // Motor Driver control pins
    Drv8231& left_motor;
    Drv8231& right_motor;

    // GPIO pins for motor direction control
    Gpio& left_in1;
    Gpio& left_in2;
    Gpio& right_in1;
    Gpio& right_in2;

    // GPIO pins for encoder channels
    Gpio& left_encoder_ch1;
    Gpio& left_encoder_ch2;
    Gpio& right_encoder_ch1;
    Gpio& right_encoder_ch2;

    uint32_t encoder_sample_us;
};

bool bsp_init(void);
Board& get_board(void);

/* PID */
std::tuple<PID&, PID&, float&, float&> get_pid_bundle();

}  // namespace MM
