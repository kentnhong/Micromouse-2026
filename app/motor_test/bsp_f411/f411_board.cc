#include <ctime>
#include <tuple>
#include "../board.h"
#include "drv8231.h"
#include "st_encoder.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

/// NOTE: 1KHZ sampling frequency for encoder to match the PID loop freq
static constexpr uint32_t kEncoderSample = 1'000;

/// GPIO settings for the F411 board
Stmf4::StGpioSettings motor_pwm_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 1};

Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 2};

Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

///GPIO: PB6 = Encoder CH1 (TIM4_CH1), PB7 = Encoder CH2 (TIM4_CH2)
//       PA2 = Motor IN1/PWM1 (TIM2_CH3), PA3 = Motor IN2/PWM2 (TIM2_CH4)

// Encoder input pins
const Stmf4::StGpioParams enc_input_params_1{6, GPIOB,
                                             enc_input_settings};  // PB6
const Stmf4::StGpioParams enc_input_params_2{7, GPIOB,
                                             enc_input_settings};  // PB7

const Stmf4::StGpioParams in1_params{
    2, GPIOA, motor_pwm_settings};  // PA2 for Motor IN1/PWM1 (TIM2_CH3)
const Stmf4::StGpioParams in2_params{
    3, GPIOA, motor_pwm_settings};  // PA3 for Motor IN2/PWM2 (TIM2_CH4)

const Stmf4::StEncoderParams encoder_params{TIM4, encoder_settings};  // TIM4

/// PWM: 1KHz frequency to match the control loop and encoder sampling rate
const Stmf4::StPwmParams pwm1_params{TIM2, Stmf4::PwmChannel::CH3, pwm_settings,
                                     32000000};
const Stmf4::StPwmParams pwm2_params{TIM2, Stmf4::PwmChannel::CH4, pwm_settings,
                                     32000000};

Stmf4::HwClk clock(Stmf4::Configuration::HSI_16MHZ);
Stmf4::HwGpio in1(in1_params);
Stmf4::HwGpio in2(in2_params);
Stmf4::HwPwm pwm1(pwm1_params);  // PA2 -> TIM2_CH3
Stmf4::HwPwm pwm2(pwm2_params);  // PA3 -> TIM2_CH4
Stmf4::HwEncoder encoder(encoder_params);
Stmf4::HwGpio encoder_ch1(enc_input_params_1);  // PB6 -> TIM4_CH1
Stmf4::HwGpio encoder_ch2(enc_input_params_2);  // PB7 -> TIM4_CH2

Drv8231 motor(pwm1, pwm2);

Board board{.encoder = encoder,
            .pwm1 = pwm1,
            .pwm2 = pwm2,
            .motor = motor,
            .in1 = in1,
            .in2 = in2,
            .encoder_ch1 = encoder_ch1,
            .encoder_ch2 = encoder_ch2,
            .encoder_sample_us = kEncoderSample};

/*************************************
* @brief All of the pins out
*        - PB6: Encoder CH1 (TIM4_CH1)
*        - PB7: Encoder CH2 (TIM4_CH2)
*        - PA2: Motor IN1/PWM1 (TIM2_CH3)
*        - PA3: Motor IN2/PWM2 (TIM2_CH4)
* @note 1. PWM frequency is set to 1KHz to match the control loop frequency and encoder sampling rate.
*       2. Using TIM4 in encoder mode to read the quadrature encoder signals for simplicity and hardware efficiency.
*/
bool bsp_init()
{
    // Enable GPIOA, GPIOB, TIM2, and TIM4 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM4EN;

    clock.init();

    // Initialize GPIOs
    in1.init();
    in2.init();

    // Initialize Motor Driver
    encoder_ch1.init();
    encoder_ch2.init();

    // Initialize PWM
    pwm1.init();
    pwm2.init();

    // Initialize Encoder
    encoder.init();
    motor.init();

    return true;
}

Board& get_board()
{
    return board;
}

}  // namespace MM