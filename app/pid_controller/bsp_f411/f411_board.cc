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
Stmf4::StGpioSettings pwm_output_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 2};

Stmf4::StGpioSettings motor_output_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

///GPIO: PA0 = Encoder CH1 (TIM2_CH1), PA1 = Encoder CH2 (TIM2_CH2)
//       PA4 = Motor IN1, PA5 = Motor IN2, PB4 = Motor PWM (TIM3_CH1)

// Encoder input pins
const Stmf4::StGpioParams enc_input_params_1{0, GPIOA,
                                             enc_input_settings};  // PA0
const Stmf4::StGpioParams enc_input_params_2{1, GPIOA,
                                             enc_input_settings};  // PA1

// Motor control pins
const Stmf4::StGpioParams in1_params{
    4, GPIOA, motor_output_settings};  // PA4 for Motor IN1
const Stmf4::StGpioParams in2_params{
    5, GPIOA, motor_output_settings};  // PA5 for Motor IN2

const Stmf4::StGpioParams pwm_output_params{4, GPIOB,
                                            pwm_output_settings};     // PB4
const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};  // TIM2

/// PWM: 1KHz frequency to match the control loop and encoder sampling rate
const Stmf4::StPwmParams pwm_params{TIM3, Stmf4::PwmChannel::CH1, pwm_settings,
                                    1000};

Stmf4::HwClk clock(Stmf4::Configuration::HSI_16MHZ);
Stmf4::HwGpio in1(in1_params);
Stmf4::HwGpio in2(in2_params);
Stmf4::HwGpio pwm_output(pwm_output_params);
Stmf4::HwPwm pwm(pwm_params);
Stmf4::HwEncoder encoder(encoder_params);
Stmf4::HwGpio encoder_ch1(enc_input_params_1);
Stmf4::HwGpio encoder_ch2(enc_input_params_2);

Drv8231 motor(in1, in2, pwm);

Board board{.encoder = encoder,
            .speed = pwm,
            .motor = motor,
            .in1 = in1,
            .in2 = in2,
            .encoder_ch1 = encoder_ch1,
            .encoder_ch2 = encoder_ch2,
            .encoder_sample_us = kEncoderSample};

/*************************************
* @brief All of the pins out
*        - PA0: Encoder CH1 (TIM2_CH1)
*        - PA1: Encoder CH2 (TIM2_CH2)
*        - PA4: Motor IN1 Encoder
*        - PA5: Motor IN2 Encoder
*        - PB4: Motor PWM (TIM3_CH1)
* @note 1. PWM frequency is set to 1KHz to match the control loop frequency and encoder sampling rate.
*       2. Using TIM2 in encoder mode to read the quadrature encoder signals for simplicity and hardware efficiency.
*/
bool bsp_init()
{
    // Enable GPIOA, GPIOB, TIM2, and TIM3 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN;

    clock.init();

    // Initialize GPIOs
    in1.init();
    in2.init();
    pwm_output.init();

    // Initialize Motor Driver
    encoder_ch1.init();
    encoder_ch2.init();

    // Initialize PWM
    pwm.init();

    // Initialize Encoder
    encoder.init();
    motor.init();

    return true;
}

Board& get_board()
{
    return board;
}

// Timer interrupt handler for control loop tick
// Making sure that the control loop runs at 1KHz
// to match the Encoder, PWM, and PID update frequencies.
volatile bool g_control_tick = false;

extern "C" void TIM4_IRQHandler(void)
{
    if (TIM4->SR & TIM_SR_UIF)
    {
        TIM4->SR &= ~TIM_SR_UIF;
        g_control_tick = true;
    }
}

}  // namespace MM
