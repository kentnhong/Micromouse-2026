#include <cstdint>
#include <span>
#include "board.h"
#include "st_gpio.h"
#include "st_sys_clock.h"
#include "st_usart.h"

namespace MM
{

// USART2 pin config
Stmf4::StGpioSettings usart_settings{
    Stmf4::GpioMode::ALTERNATE, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NONE, 7};

Stmf4::StGpioParams tx_params{2, GPIOA, usart_settings};  // PA2 USART2_TX
Stmf4::StGpioParams rx_params{3, GPIOA, usart_settings};  // PA3 USART2_RX

Stmf4::HwGpio tx_gpio(tx_params);
Stmf4::HwGpio rx_gpio(rx_params);

Stmf4::HwClk clock{};

Board board{.usart = usart, .rx = rx_gpio, .tx = tx_gpio};

bool bsp_init()
{
    bool ret = true;

    // SysClock config
    ret &= clock.init(Stmf4::HwClock::configuration::DEFAULT_4MHZ);

    // Enable peripheral clocks for USART2 and GPIOA
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    // Initialize USART and pins
    ret &= tx_gpio.init();
    ret &= rx_gpio.init();
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
    // This is for getting the first byte
    if (usart.get_addr()->SR & USART_SR_RXNE)
    {
        if (board.usart.receive(rx_byte))
        {
            // received start ready 1 byte, echo it back
            std::span<const uint8_t> tx_span(&rx_byte, 1);
            board.usart.transfer(tx_span);
        }
    }
}

}  // namespace MM
