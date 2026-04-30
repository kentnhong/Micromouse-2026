#include "../board.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_timebase.h"

// Global flag
bool set = false;

namespace
{
static constexpr uint32_t kTimerFreq{1'000'000};
static constexpr std::chrono::microseconds kTimerPeriod{100};
};  // namespace

namespace MM
{
namespace Stmf4
{
StGpioSettings gpio_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                             GpioOspeed::HIGH, GpioPupd::NO_PULL, 0};
StGpioParams gpio_params{5, GPIOA, gpio_settings};

StTimebaseParams timebase_params{TIM5};

HwGpio gpio{gpio_params};
HwTimebase timebase{timebase_params};
HwClk sysclk{Configuration::HSI_16MHZ};
};  // namespace Stmf4

Board board{.pin = Stmf4::gpio, .counter = Stmf4::timebase};

bool board_init()
{
    bool result = true;

    // Init sysclk
    result = result && Stmf4::sysclk.init();
    uint32_t hclk = Stmf4::sysclk.get_freq();

    // Periph clock inits
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // Peripheral inits
    result = result && Stmf4::gpio.init();
    result =
        result && Stmf4::timebase.init(hclk, kTimerFreq, kTimerPeriod, true);

    return result;
}

Board& get_board()
{
    return board;
}

extern "C" void TIM5_IRQHandler()
{
    bool result = Stmf4::gpio.toggle();
    set ^= result;
}
};  // namespace MM