// STM32F4 blink board implementation matching LBR L4 style
#include <stdint.h>
#include "../../../common/drivers/platform/stm32f4/st_gpio.h"
#include "../../../mcu_support/stm32/f4xx/stm32f4xx.h"
#include "board.h"
#include "st_sys_clk.h"

namespace MM
{

Stmf4::StGpioSettings led_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 0};

Stmf4::StGpioParams led_a_params{3,      // pin_num
                                 GPIOC,  // base_addr
                                 led_settings};
Stmf4::StGpioParams led_b_params{4,      // pin_num
                                 GPIOC,  // base_addr
                                 led_settings};
Stmf4::StGpioParams led_c_params{5,      // pin_num
                                 GPIOC,  // base_addr
                                 led_settings};

Stmf4::HwGpio led_a{led_a_params};
Stmf4::HwGpio led_b{led_b_params};
Stmf4::HwGpio led_c{led_c_params};

Board board{.led_a = led_a, .led_b = led_b, .led_c = led_c};

Stmf4::HwClk clock{MM::Stmf4::Configuration::SYSCLK_HSE_24MHZ};

bool board_init()
{
    bool return_val = true;
    clock.init();
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    return_val &= led_a.init();
    return_val &= led_b.init();
    return_val &= led_c.init();

    return return_val;
}

Board& get_board(void)
{
    return board;
}

}  // namespace MM
