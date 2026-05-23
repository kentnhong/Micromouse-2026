#include <stdint.h>
#include "../../../common/core/algorithm/floodfill.h"
#include "../board.h"
#include "st_gpio.h"
#include "st_sys_clk.h"

namespace MM
{

namespace
{

// Update these mappings to match your board wiring.
constexpr uint8_t kLed1Pin{3};          // PC3
constexpr uint8_t kLed2Pin{4};          // PC4
constexpr uint8_t kLed3Pin{5};          // PC5
constexpr uint8_t kSearchButtonPin{4};  // PB4
constexpr uint8_t kZoomButtonPin{12};   // PA12

Stmf4::StGpioSettings led_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 0};

Stmf4::StGpioSettings button_settings{
    Stmf4::GpioMode::GPI, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 0};

Stmf4::StGpioParams led1_params{kLed1Pin, GPIOC, led_settings};
Stmf4::StGpioParams led2_params{kLed2Pin, GPIOC, led_settings};
Stmf4::StGpioParams led3_params{kLed3Pin, GPIOC, led_settings};
Stmf4::StGpioParams search_bt_params{kSearchButtonPin, GPIOB, button_settings};
Stmf4::StGpioParams zoom_bt_params{kZoomButtonPin, GPIOA, button_settings};

Stmf4::HwGpio led1{led1_params};
Stmf4::HwGpio led2{led2_params};
Stmf4::HwGpio led3{led3_params};
Stmf4::HwGpio search_bt{search_bt_params};
Stmf4::HwGpio zoom_bt{zoom_bt_params};

MM::Floodfill floodfill;
MM::Cli cli{MM::CliParams{.led1 = led1,
                          .led2 = led2,
                          .led3 = led3,
                          .search_bt = search_bt,
                          .zoom_bt = zoom_bt,
                          .floodfill = floodfill}};

Board board{.cli = cli};

Stmf4::HwClk clock{MM::Stmf4::Configuration::SYSCLK_HSE_100MHZ};

}  // namespace

bool board_init()
{
    bool return_val = true;

    clock.init();

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;

    return_val &= led1.init();
    return_val &= led2.init();
    return_val &= led3.init();
    return_val &= search_bt.init();
    return_val &= zoom_bt.init();

    led1.set(false);
    led2.set(false);
    led3.set(false);

    return return_val;
}

Board& get_board(void)
{
    return board;
}

}  // namespace MM
