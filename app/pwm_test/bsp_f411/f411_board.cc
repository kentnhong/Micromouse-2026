#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_pwm.h"

namespace MM
{

// Output pin config for Timer 3, Channel 1 (PB4, D5 on Nucleo F411RE)
Stmf4::StGpioSettings pwm_output_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 2}; // AF2 for TIM3 on PB4    

const Stmf4::StGpioParams pwm_output_params{4, GPIOB, pwm_output_settings};

// PWM Config (TIM3 CH1)
Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

const Stmf4::StPwmParams pwm_params{TIM3, 1, pwm_settings};

// Create PWM GPIO and PWM objects
Stmf4::HwGpio pwm_output(pwm_output_params);
Stmf4::HwPwm pwm(pwm_params);

Board board{.pwm = pwm};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;

    // Initialize PWM and pins
    bool ret = true;

    ret = ret && pwm_output.init();
    ret = ret && pwm.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM