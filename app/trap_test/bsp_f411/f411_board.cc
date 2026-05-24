/**
 * @file f411_board.cc
 * @brief Trap test BSP — MotionController, USART1, start button.
 * @author Bex Saw
 */

#include <cstdint>
#include "../board.h"
#include "delay.h"
#include "drv8231.h"
#include "motioncontroller.h"
#include "st_encoder.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"
#include "st_timebase.h"
#include "st_usart.h"

namespace
{
static constexpr uint32_t kEncoderSampleUs{5'000};
static constexpr uint32_t kPwmFreqHz{323};
}  // namespace

namespace MM
{
namespace Stmf4
{

/* ── Clock ─────────────────────────────────────────────────────────────── */
HwClk clk{Configuration::SYSCLK_HSE_100MHZ_24MHZ_INPUT};

/* ── Motor PWM GPIO (AF, TIM2) ────────────────────────────────────────── */
StGpioSettings motor_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                  GpioOspeed::LOW, GpioPupd::NO_PULL, 1};
StGpioParams in1_left_params{2, GPIOA, motor_af_settings};   // PA2 TIM2_CH3
StGpioParams in2_left_params{3, GPIOA, motor_af_settings};   // PA3 TIM2_CH4
StGpioParams in1_right_params{15, GPIOA, motor_af_settings}; // PA15 TIM2_CH1
StGpioParams in2_right_params{3, GPIOB, motor_af_settings};  // PB3  TIM2_CH2
HwGpio in1_left{in1_left_params};
HwGpio in2_left{in2_left_params};
HwGpio in1_right{in1_right_params};
HwGpio in2_right{in2_right_params};

/* ── Encoder GPIO (AF, TIM3/TIM4) ────────────────────────────────────── */
StGpioSettings enc_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                GpioOspeed::LOW, GpioPupd::PULL_UP, 2};
StGpioParams enc_left_ch1_params{6, GPIOB, enc_af_settings};  // PB6 TIM4_CH1
StGpioParams enc_left_ch2_params{7, GPIOB, enc_af_settings};  // PB7 TIM4_CH2
StGpioParams enc_right_ch1_params{6, GPIOC, enc_af_settings}; // PC6 TIM3_CH1
StGpioParams enc_right_ch2_params{5, GPIOB, enc_af_settings}; // PB5 TIM3_CH2
HwGpio enc_left_ch1{enc_left_ch1_params};
HwGpio enc_left_ch2{enc_left_ch2_params};
HwGpio enc_right_ch1{enc_right_ch1_params};
HwGpio enc_right_ch2{enc_right_ch2_params};

/* ── PWM ──────────────────────────────────────────────────────────────── */
StPwmSettings pwm_settings{PwmMode::EDGE_ALIGNED, PwmOutputMode::PWM_MODE_1,
                            PwmDir::UPCOUNTING};
StPwmParams pwm1_left_params{TIM2, PwmChannel::CH3, pwm_settings, 100'000'000};
StPwmParams pwm2_left_params{TIM2, PwmChannel::CH4, pwm_settings, 100'000'000};
StPwmParams pwm1_right_params{TIM2, PwmChannel::CH1, pwm_settings, 100'000'000};
StPwmParams pwm2_right_params{TIM2, PwmChannel::CH2, pwm_settings, 100'000'000};
HwPwm pwm1_left{pwm1_left_params};
HwPwm pwm2_left{pwm2_left_params};
HwPwm pwm1_right{pwm1_right_params};
HwPwm pwm2_right{pwm2_right_params};

/* ── Encoders ─────────────────────────────────────────────────────────── */
StEncoderSettings enc_settings_left{.mode = EncMode::MODE_3,
                                     .channel = EncChannel::BOTH,
                                     .polarity = EncInputPolarity::RISING,
                                     .slave_mode = EncSlaveMode::DISABLED,
                                     .invert_direction = false};
StEncoderSettings enc_settings_right{.mode = EncMode::MODE_3,
                                      .channel = EncChannel::BOTH,
                                      .polarity = EncInputPolarity::RISING,
                                      .slave_mode = EncSlaveMode::DISABLED,
                                      .invert_direction = false};
StEncoderParams encoder_left_params{TIM4, enc_settings_left};
StEncoderParams encoder_right_params{TIM3, enc_settings_right};
HwEncoder encoder_left{encoder_left_params};
HwEncoder encoder_right{encoder_right_params};

/* ── Motor drivers ────────────────────────────────────────────────────── */
Drv8231 motor_left{pwm2_left, pwm1_left};
Drv8231 motor_right{pwm1_right, pwm2_right};

/* ── USART1 (PA9=TX, PA10=RX, AF7) ───────────────────────────────────── */
StGpioSettings usart_gpio_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                    GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 7};
StGpioParams usart_tx_params{9, GPIOA, usart_gpio_settings};
StGpioParams usart_rx_params{10, GPIOA, usart_gpio_settings};
HwGpio usart_tx{usart_tx_params};
HwGpio usart_rx{usart_rx_params};
StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};
StUsartParams usart_params{USART1, 100'000'000u, 115200, usart_settings};
StUsart usart{usart_params};

/* ── Start button (PB4, active low) ──────────────────────────────────── */
StGpioSettings button_settings{GpioMode::GPI, GpioOtype::PUSH_PULL,
                                GpioOspeed::LOW, GpioPupd::PULL_UP, 0};
StGpioParams start_bt_params{4, GPIOB, button_settings};
HwGpio start_bt{start_bt_params};

/* ── Delay timer (TIM5) ───────────────────────────────────────────────── */
StTimebaseParams delay_params{TIM5};
HwTimebase delay_timer{delay_params};

}  // namespace Stmf4

/* ── MotionController ─────────────────────────────────────────────────── */
static constexpr Gains kGains{.kp = 0.000018f, .ki = 0.0f, .kd = 0.000018f};
MotionControllerParams mc_params{.motor_left       = Stmf4::motor_left,
                                  .motor_right      = Stmf4::motor_right,
                                  .encoder_left     = Stmf4::encoder_left,
                                  .encoder_right    = Stmf4::encoder_right,
                                  .gains            = kGains,
                                  .encoder_sample_us = kEncoderSampleUs};
MotionController motion_controller{mc_params};

/* ── Board ────────────────────────────────────────────────────────────── */
Board board{.motion    = motion_controller,
            .usart     = Stmf4::usart,
            .start_bt  = Stmf4::start_bt};

/* ── BSP init ─────────────────────────────────────────────────────────── */
bool bsp_init()
{
    bool ret = true;

    ret = ret && Stmf4::clk.init();

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN |
                    RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    /* Motors */
    ret = ret && Stmf4::in1_left.init();
    ret = ret && Stmf4::in2_left.init();
    ret = ret && Stmf4::in1_right.init();
    ret = ret && Stmf4::in2_right.init();
    ret = ret && Stmf4::pwm1_left.init();
    ret = ret && Stmf4::pwm2_left.init();
    ret = ret && Stmf4::pwm1_right.init();
    ret = ret && Stmf4::pwm2_right.init();
    ret = ret && Stmf4::pwm1_left.set_frequency(kPwmFreqHz);
    ret = ret && Stmf4::motor_left.init();
    ret = ret && Stmf4::motor_right.init();

    /* Encoders */
    ret = ret && Stmf4::enc_left_ch1.init();
    ret = ret && Stmf4::enc_left_ch2.init();
    ret = ret && Stmf4::encoder_left.init();
    ret = ret && Stmf4::enc_right_ch1.init();
    ret = ret && Stmf4::enc_right_ch2.init();
    ret = ret && Stmf4::encoder_right.init();

    /* USART */
    ret = ret && Stmf4::usart_tx.init();
    ret = ret && Stmf4::usart_rx.init();
    ret = ret && Stmf4::usart.init();

    /* Button */
    ret = ret && Stmf4::start_bt.init();

    /* Delay timer: TIM5 at 1 MHz */
    ret = ret && Stmf4::delay_timer.init(50'000'000u, 1'000'000u,
                                          std::chrono::microseconds(4'294'967u));
    Stmf4::delay_timer.start();
    Utils::bind_timebase(Stmf4::delay_timer);

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
