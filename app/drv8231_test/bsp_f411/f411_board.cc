#include "../board.h"
#include "drv8231.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

// PA2 and PA3 drive the DRV8231 IN/IN inputs directly with PWM.
Stmf4::StGpioSettings motor_pwm_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 1};

// PWM Config
Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

const Stmf4::StGpioParams in1_params{2, GPIOA,
                                     motor_pwm_settings};  // PA2 TIM2_CH3
const Stmf4::StGpioParams in2_params{3, GPIOA,
                                     motor_pwm_settings};  // PA3 TIM2_CH4
const Stmf4::StPwmParams pwm1_params{TIM2, Stmf4::PwmChannel::CH3, pwm_settings,
                                     32000000};
const Stmf4::StPwmParams pwm2_params{TIM2, Stmf4::PwmChannel::CH4, pwm_settings,
                                     32000000};

Stmf4::HwGpio in1(in1_params);
Stmf4::HwGpio in2(in2_params);
Stmf4::HwPwm pwm1(pwm1_params);
Stmf4::HwPwm pwm2(pwm2_params);

Drv8231 drv8231(pwm1, pwm2);
Board board = {.drv8231 = drv8231,
               .in1 = in1,
               .in2 = in2,
               .pwm1 = pwm1,
               .pwm2 = pwm2};

bool bsp_init()
{

    // Initialize system clock
    // clock.init();

    // Enable GPIOA and TIM2 for PA2/PA3 PWM.
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Initialize GPIO pins for IN1 IN2 and PWM
    bool ret = true;

    ret = ret && in1.init();
    ret = ret && in2.init();
    ret = ret && pwm1.init();
    ret = ret && pwm2.init();

    // Initialize DRV8231 object
    ret = ret && drv8231.init();

    return true;
}

Board& get_board()
{
    return board;
}
}  // namespace MM
