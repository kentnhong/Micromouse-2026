// STM32F4 blink board implementation matching LBR L4 style
#include <stdint.h>
#include "../../../common/drivers/platform/stm32f4/st_gpio.h"
#include "../../../mcu_support/stm32/f4xx/stm32f4xx.h"
#include "board.h"

namespace MM
{

Stmf4::StGpioSettings led_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 0};

Stmf4::StGpioParams led_params{5,      // pin_num
                               GPIOA,  // base_addr
                               led_settings};

Stmf4::HwGpio led{led_params};

Board board{.led = static_cast<Gpio&>(led)};

bool board_init()
{
    bool return_val = true;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    return_val &= led.init();
    return return_val;
}

Board& get_board(void)
{
    return board;
}

}  // namespace MM
