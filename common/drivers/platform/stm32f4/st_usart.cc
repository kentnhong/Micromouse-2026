#include "st_usart.h"

namespace MM
{
namespace Stmf4
{

static inline uint32_t usartdiv_calc(uint32_t fck, uint32_t baud, bool over8)
{
    uint32_t oversample = over8 ? 8 : 16;

    uint32_t div = fck / (oversample * baud);
    uint32_t rem = fck % (oversample * baud);

    uint32_t frac_scale = over8 ? 8 : 16;
    uint32_t frac =
        (rem * frac_scale + (oversample * baud) / 2) / (oversample * baud);

    if (frac == frac_scale)
    {
        frac = 0;
        div++;
    }

    if (over8)
        frac &= 0x7;

    return (div << 4) | frac;
}

StUsart::StUsart(StUsartParams& params_)
    : base_addr{params_.base_addr},
      settings{params_.settings},
      clock_freq{params_.clock_freq},
      baud_rate{params_.baud_rate}
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

bool StUsart::send(std::span<const uint8_t> txbuf)
{
    size_t size = 0;
    for (const auto& byte : txbuf)
    {
        // Wait until transmit data register is empty
        while (!(base_addr->SR & USART_SR_TXE))
        {
        }

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
    if (base_addr == nullptr)
    {
        return false;
    }

    // Disable USART before configuration
    base_addr->CR1 &= ~USART_CR1_UE;

    // Clear word length and parity bits for 8N1
    base_addr->CR1 &= ~USART_CR1_M;

    // Set Oversampling rate
    bool over8;
    if (settings.oversample == UsartOversample::X16)
    {
        base_addr->CR1 &= ~USART_CR1_OVER8;
        over8 = false;
    }
    else
    {
        base_addr->CR1 |= USART_CR1_OVER8;
        over8 = true;
    }

    // Three sample bit method where we take the value of the majority of the 3 middle samples inside the 16 samples
    if (settings.sample_mode == UsartSampleMode::MAJORITY)
        base_addr->CR3 &= ~USART_CR3_ONEBIT;
    else
        base_addr->CR3 |= USART_CR3_ONEBIT;

    // Calculate usartdiv
    uint16_t usartdiv = usartdiv_calc(clock_freq, baud_rate, over8);

    // Set baud rate
    base_addr->BRR = usartdiv;

    // Set 1 stop bit
    base_addr->CR2 &= ~USART_CR2_STOP;

    // Enable transmitter, receiver, and USART
    base_addr->CR1 |= (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE);

    // Enable RXNE interrupt
    base_addr->CR1 |= USART_CR1_RXNEIE;

    // (Optional) Disable DMA unless needed
    base_addr->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);

    return true;
}

USART_TypeDef* StUsart::get_addr()
{
    return this->base_addr;
}
}  // namespace Stmf4
}  // namespace MM