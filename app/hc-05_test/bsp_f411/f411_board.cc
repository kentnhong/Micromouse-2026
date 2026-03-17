#include <cstdint>
#include <span>
#include "board.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_usart.h"

namespace MM
{

// USART2 pin config
Stmf4::StGpioSettings gpio_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::HIGH,
    Stmf4::GpioPupd::NO_PULL, 7};

// KEY Pin config HC-05 KEY pin is connected to PA12
Stmf4::StGpioSettings key_gpio_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 12};

//Stmf4::StGpioParams key_params{12, GPIOA, key_gpio_settings};  // PA12 KEY
Stmf4::StGpioParams tx_params{11, GPIOA, gpio_settings};  // PA11 USART2_TX
Stmf4::StGpioParams rx_params{7, GPIOC, gpio_settings};   // PC7 USART2_RX

Stmf4::HwGpio tx_gpio(tx_params);
Stmf4::HwGpio rx_gpio(rx_params);
//Stmf4::HwGpio key_gpio(key_params);  // GPIO for controlling HC-05 mode (KEY pin)

Stmf4::HwClk clock{Stmf4::Configuration::HSI_16MHZ};

Stmf4::StUsartSettings usart_settings{Stmf4::UsartOversample::X16,
                                      Stmf4::UsartSampleMode::MAJORITY};

Stmf4::StUsartParams usart_params{
    USART2,
    0,
    9600,
    usart_settings,
};

Stmf4::StUsart usart{usart_params};
Board board{.usart = usart, .rx = rx_gpio, .tx = tx_gpio};

bool bsp_init()
{
    bool ret = true;

    ret &= clock.init();
    usart.set_clock_freq(clock.get_freq());

    // Enable peripheral clocks for GPIOA, USART1, and USART2
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    RCC->APB2ENR |= RCC_APB2ENR_USART1EN;

    // Initialize USART and pins
    ret &= tx_gpio.init();
    ret &= rx_gpio.init();
    //ret &= key_gpio.init();  // Initialize KEY pin (for controlling HC-05 mode)
    ret &= usart.init();

    // Enable USART2 interrupt in NVIC
    NVIC_SetPriority(USART2_IRQn, 0);
    NVIC_EnableIRQ(USART2_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

// Make sure to clear the ORE flag in the USART2 interrupt handler to prevent it
// from blocking further interrupts.
extern "C" void USART2_IRQHandler(void)
{
    // Check if data is available
    if (usart.get_addr()->SR & USART_SR_RXNE)
    {
        if (board.usart.receive(rx_byte))
        {
            // received 1 byte, echo it back
            std::span<const uint8_t> tx_span(&rx_byte, 1);
            board.usart.send(tx_span);
        }
    }
}

}  // namespace MM
