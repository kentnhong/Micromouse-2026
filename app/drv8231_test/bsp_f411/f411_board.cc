#include "../board.h"
#include "drv8231.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

Stmf4::StGpioSettings motor_pwm_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 1};

Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

/// MOTOR: RIGHT — PA2 (TIM2_CH3), PA3 (TIM2_CH4)
const Stmf4::StGpioParams in1_params_left{2, GPIOA, motor_pwm_settings};  // PA2
const Stmf4::StGpioParams in2_params_left{3, GPIOA, motor_pwm_settings};  // PA3
const Stmf4::StPwmParams pwm1_params_left{TIM2, Stmf4::PwmChannel::CH3,
                                          pwm_settings, 32000000};
const Stmf4::StPwmParams pwm2_params_left{TIM2, Stmf4::PwmChannel::CH4,
                                          pwm_settings, 32000000};

/// MOTOR: LEFT — PA15 (TIM2_CH1), PB3 (TIM2_CH2)
const Stmf4::StGpioParams in1_params_right{3, GPIOB,
                                           motor_pwm_settings};  // PB3
const Stmf4::StGpioParams in2_params_right{15, GPIOA,
                                           motor_pwm_settings};  // PA15
const Stmf4::StPwmParams pwm1_params_right{TIM2, Stmf4::PwmChannel::CH2,
                                           pwm_settings, 32000000};
const Stmf4::StPwmParams pwm2_params_right{TIM2, Stmf4::PwmChannel::CH1,
                                           pwm_settings, 32000000};

Stmf4::HwGpio in1_left(in1_params_left);
Stmf4::HwGpio in2_left(in2_params_left);
Stmf4::HwGpio in1_right(in1_params_right);
Stmf4::HwGpio in2_right(in2_params_right);

Stmf4::HwPwm pwm1_left(pwm1_params_left);
Stmf4::HwPwm pwm2_left(pwm2_params_left);
Stmf4::HwPwm pwm1_right(pwm1_params_right);
Stmf4::HwPwm pwm2_right(pwm2_params_right);

Drv8231 drv8231_left(pwm1_left, pwm2_left);
Drv8231 drv8231_right(pwm1_right, pwm2_right);

Board board{.drv8231_left = drv8231_left,
            .drv8231_right = drv8231_right,
            .pwm1_left = pwm1_left,
            .pwm2_left = pwm2_left,
            .pwm1_right = pwm1_right,
            .pwm2_right = pwm2_right};

bool bsp_init()
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    bool ret = true;

    ret = ret && in1_left.init();
    ret = ret && in2_left.init();
    ret = ret && in1_right.init();
    ret = ret && in2_right.init();

    ret = ret && pwm1_left.init();
    ret = ret && pwm2_left.init();
    ret = ret && pwm1_right.init();
    ret = ret && pwm2_right.init();

    ret = ret && drv8231_left.init();
    ret = ret && drv8231_right.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
