#include "../board.h"
#include "drv8231.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

// IN1 and IN2 act as a 1 or 0 to control direction, while PWM controls speed

Stmf4::StGpioSettings in1_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

const Stmf4::StGpioParams in1_params{0, GPIOA, in1_settings};  // PA0

Stmf4::StGpioSettings in2_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

const Stmf4::StGpioParams in2_params{1, GPIOA, in2_settings};  // PA1

// PWM Config
Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

const Stmf4::StPwmParams pwm_params{TIM3, Stmf4::PwmChannel::CH1, pwm_settings};

Stmf4::HwClk clock{MM::Stmf4::Configuration::HSI_16MHZ};

Stmf4::HwGpio in1(in1_params);
Stmf4::HwGpio in2(in2_params);
Stmf4::HwPwm pwm(pwm_params);

Drv8231 drv8231(in1, in2, pwm);
Board board = {.drv8231 = drv8231, .in1 = in1, .in2 = in2, .pwm = pwm};

bool bsp_init()
{

    // Initialize system clock
    clock.init();

    // Enable GPIO and PWM clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Initialize GPIO pins for IN1 IN2 and PWM
    bool ret = true;

    ret = ret && in1.init();
    ret = ret && in2.init();
    ret = ret && pwm.init();

    // Initialize DRV8231 object
    ret = ret && drv8231.init();

    return true;
}

Board& get_board()
{
    return board;
}
}  // namespace MM