#include <span>
#include "../board.h"
#include "drv8231.h"
#include "st_encoder.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"
#include "st_usart.h"

namespace
{
static constexpr uint32_t kEncoderSampleUs{
    20'000};  // 20 ms — ~8 ticks/sample at 100 mm/s
}

namespace MM
{
namespace Stmf4
{

StGpioSettings motor_pwm_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                  GpioOspeed::LOW, GpioPupd::NO_PULL, 1};

StGpioSettings enc_input_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                  GpioOspeed::LOW, GpioPupd::PULL_UP, 2};

StGpioSettings usart_gpio_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                   GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 7};

StPwmSettings pwm_settings{PwmMode::EDGE_ALIGNED, PwmOutputMode::PWM_MODE_1,
                           PwmDir::UPCOUNTING};

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

StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};

// Left motor:  PA2 = TIM2_CH3, PA3 = TIM2_CH4
// Left encoder: PB7 = TIM4_CH2 (A), PB6 = TIM4_CH1 (B)
StGpioParams in1_left_params{2, GPIOA, motor_pwm_settings};
StGpioParams in2_left_params{3, GPIOA, motor_pwm_settings};
StGpioParams enc_left_ch1_params{6, GPIOB, enc_input_settings};  // PB6 = B
StGpioParams enc_left_ch2_params{7, GPIOB, enc_input_settings};  // PB7 = A

// Right motor: PA15 = TIM2_CH1, PB3 = TIM2_CH2
// Right encoder: PC6 = TIM3_CH1 (A), PB5 = TIM3_CH2 (B)
StGpioParams in1_right_params{15, GPIOA, motor_pwm_settings};
StGpioParams in2_right_params{3, GPIOB, motor_pwm_settings};
StGpioParams enc_right_ch1_params{6, GPIOC, enc_input_settings};  // PC6 = A
StGpioParams enc_right_ch2_params{5, GPIOB, enc_input_settings};  // PB5 = B

// USART1: PA9 = TX, PA10 = RX (AF7)
StGpioParams usart_tx_params{9, GPIOA, usart_gpio_settings};
StGpioParams usart_rx_params{10, GPIOA, usart_gpio_settings};

StEncoderParams encoder_left_params{TIM4, enc_settings_left};
StEncoderParams encoder_right_params{TIM3, enc_settings_right};

StPwmParams pwm1_left_params{TIM2, PwmChannel::CH3, pwm_settings, 32000000};
StPwmParams pwm2_left_params{TIM2, PwmChannel::CH4, pwm_settings, 32000000};
StPwmParams pwm1_right_params{TIM2, PwmChannel::CH1, pwm_settings, 32000000};
StPwmParams pwm2_right_params{TIM2, PwmChannel::CH2, pwm_settings, 32000000};

HwClk clk{Configuration::HSI_16MHZ};
StUsartParams usart_params{USART1, clk.get_freq(), 115200, usart_settings};

HwGpio in1_left{in1_left_params};
HwGpio in2_left{in2_left_params};
HwPwm pwm1_left{pwm1_left_params};
HwPwm pwm2_left{pwm2_left_params};
HwEncoder encoder_left{encoder_left_params};
HwGpio enc_left_ch1{enc_left_ch1_params};
HwGpio enc_left_ch2{enc_left_ch2_params};

HwGpio in1_right{in1_right_params};
HwGpio in2_right{in2_right_params};
HwPwm pwm1_right{pwm1_right_params};
HwPwm pwm2_right{pwm2_right_params};
HwEncoder encoder_right{encoder_right_params};
HwGpio enc_right_ch1{enc_right_ch1_params};
HwGpio enc_right_ch2{enc_right_ch2_params};

HwGpio usart_tx{usart_tx_params};
HwGpio usart_rx{usart_rx_params};
StUsart usart{usart_params};

}  // namespace Stmf4

Drv8231 motor_left(Stmf4::pwm1_left, Stmf4::pwm2_left);
Drv8231 motor_right(Stmf4::pwm2_right, Stmf4::pwm1_right);

/*
 * Pin mapping
 *  LEFT  — PA2(TIM2_CH3) PA3(TIM2_CH4) | PB7(TIM4_CH2 A) PB6(TIM4_CH1 B)
 *  RIGHT — PA15(TIM2_CH1) PB3(TIM2_CH2) | PC6(TIM3_CH1 A) PB5(TIM3_CH2 B)
 *  USART1 — PA9(TX) PA10(RX)
 */
Board board{.encoder_left = Stmf4::encoder_left,
            .pwm1_left = Stmf4::pwm1_left,
            .pwm2_left = Stmf4::pwm2_left,
            .motor_left = motor_left,
            .in1_left = Stmf4::in1_left,
            .in2_left = Stmf4::in2_left,
            .enc_left_ch1 = Stmf4::enc_left_ch1,
            .enc_left_ch2 = Stmf4::enc_left_ch2,

            .encoder_right = Stmf4::encoder_right,
            .pwm1_right = Stmf4::pwm1_right,
            .pwm2_right = Stmf4::pwm2_right,
            .motor_right = motor_right,
            .in1_right = Stmf4::in1_right,
            .in2_right = Stmf4::in2_right,
            .enc_right_ch1 = Stmf4::enc_right_ch1,
            .enc_right_ch2 = Stmf4::enc_right_ch2,

            .usart = Stmf4::usart,
            .encoder_sample_us = kEncoderSampleUs};

bool bsp_init()
{
    bool ret = true;

    ret = ret && Stmf4::clk.init();
    ret = ret && Stmf4::usart.set_clock_freq(Stmf4::clk.get_freq());

    RCC->AHB1ENR |=
        RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN;
    RCC->APB1ENR |=
        RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_TIM4EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    ret = ret && Stmf4::in1_left.init();
    ret = ret && Stmf4::in2_left.init();
    ret = ret && Stmf4::enc_left_ch1.init();
    ret = ret && Stmf4::enc_left_ch2.init();
    ret = ret && Stmf4::encoder_left.init();
    ret = ret && Stmf4::pwm1_left.init();
    ret = ret && Stmf4::pwm2_left.init();
    ret = ret && motor_left.init();

    ret = ret && Stmf4::in1_right.init();
    ret = ret && Stmf4::in2_right.init();
    ret = ret && Stmf4::enc_right_ch1.init();
    ret = ret && Stmf4::enc_right_ch2.init();
    ret = ret && Stmf4::encoder_right.init();
    ret = ret && Stmf4::pwm1_right.init();
    ret = ret && Stmf4::pwm2_right.init();
    ret = ret && motor_right.init();

    ret = ret && Stmf4::usart_tx.init();
    ret = ret && Stmf4::usart_rx.init();
    ret = ret && Stmf4::usart.init();

    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

extern "C" void USART1_IRQHandler(void)
{
    if (Stmf4::usart.get_addr()->SR & USART_SR_RXNE)
    {
        uint8_t byte = 0;
        if (board.usart.receive(byte))
        {
            board.usart.send(std::span<const uint8_t>(&byte, 1));
        }
    }
}

}  // namespace MM
