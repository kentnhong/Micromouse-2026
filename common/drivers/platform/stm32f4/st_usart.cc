#include "st_usart.h"

namespace MM
{
namespace Stmf4
{

StUsart::StUsart(USART_TypeDef* base_addr, uint32_t sys_clk,
                 uint32_t clock_freq)
    : base_addr(base_addr), uartdiv(sys_clk / clock_freq)
{
}

bool StUsart::receive(uint8_t* data, size_t length)
{

    // If theirs an overrun error, it will be cleared
    if (base_addr->SR & USART_SR_ORE)
    {
        // Clear ORE flag by reading SR and DR
        volatile uint32_t temp = base_addr->SR;
        temp = base_addr->DR;
        (void)temp;  // Avoid unused variable warning
    }

    // Read data from USART data register and store in buffer
    for (size_t i = 0; i < length; i++)
    {
        // Wait until receive data register is not empty
        while (!(base_addr->SR & USART_SR_RXNE));

        // Read byte from data register
        data[i] = base_addr->DR & 0xFF;
    }

    return true;
}

bool StUsart::transfer(std::span<const uint8_t> txbuf)
{
    size_t size = 0;
    for (const auto& byte : txbuf)
    {
        // Wait until transmit data register is empty
        while (!(base_addr->SR & USART_SR_TXE));

        // Write byte to data register
        base_addr->DR = byte;
        size++;
    }
    // Wait until transmission is complete TC=1
    while (!(base_addr->SR & USART_SR_TC));

    return true;
}

bool StUsart::init()
{
    // Clear bits, word length, and parity to default (clear M1 and M0 for 8 data bits)
    base_addr->CR1 &= ~USART_CR1_UE | ~USART_CR1_M | ~USART_CR1_PCE;

    // Set baud rate
    base_addr->BRR = uartdiv;

    // Set stop bits to 1
    base_addr->CR2 &= ~USART_CR2_STOP;

    // Enable USART peripheral, transmitter, and receiver
    base_addr->CR1 |= USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

    // DMAT and DMAR can be configured (for highspeed data transfer using DMA)
    base_addr->CR3 |= USART_CR3_DMAT | USART_CR3_DMAR;

    // Enable USART interrupts RXNE (Receive Data register not empty) interrupt
    base_addr->CR1 |= USART_CR1_RXNEIE;
    return true;
}

}  // namespace Stmf4
}  // namespace MM