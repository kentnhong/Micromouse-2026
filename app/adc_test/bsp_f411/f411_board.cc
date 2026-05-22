#include <array>
#include "board.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_usart.h"

// ADC IRQ will set this to true if there is overrun, then we can just handle it with board_recover()
volatile bool g_adc_ovr = false;

namespace MM
{
namespace Stmf4
{
StGpioSettings ir_led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                               GpioOspeed::MEDIUM, GpioPupd::PULL_DOWN, 0};
StGpioParams ir_led_params{6, GPIOA, ir_led_settings};
HwGpio ir_led{ir_led_params};

// Pair 1 test: emitter on PA7, phototransistor input on PB1 (ADC1_IN9).
StGpioSettings phototrans_settings{GpioMode::ANALOG, GpioOtype::PUSH_PULL,
                                   GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioParams phototrans_params{0, GPIOB, phototrans_settings};
HwGpio phototrans{phototrans_params};

StDmaSettings dma_settings{DmaChSel::CH0, DmaPriority::VERY_HIGH,
                           DmaWidth::HALF_WORD, DmaDataDir::PERIPH_TO_MEM};
StDmaParams dma_params{
    dma_settings, DMA2, DMA2_Stream0,
    static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&ADC1->DR))};
HwDma dma{dma_params};

std::array<uint8_t, 1> adc_seq{8};
AdcChCycles ch_cycles{.ch = 8, .cycles = AdcCycles::CYCLES_144};
std::array<AdcChCycles, 1> adc_ch_cycles{ch_cycles};

StAdcSettings adc_settings{AdcResolution::TWELVE_BIT,
                           AdcClkPrescaler::PCLK2_DIV_2,
                           AdcTriggerSource::SOFTWARE,
                           AdcOverrunInt::OVRIE_DIS,
                           AdcDma::DMA_ENABLE,
                           adc_seq,
                           adc_ch_cycles};
StAdcParams adc_params{adc_settings, ADC1, ADC1_COMMON};
HwAdc adc{adc_params};

HwClk clk{Configuration::SYSCLK_HSE_100MHZ};

// USART settings
StGpioSettings tx_settings{GpioMode::AF, GpioOtype::PUSH_PULL, GpioOspeed::LOW,
                           GpioPupd::NO_PULL, 7};
StGpioParams tx_params{9, GPIOA, tx_settings};  // PA9 USART1_TX
HwGpio tx{tx_params};
StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};
StUsartParams usart_params{USART1, clk.get_freq(), 115200, usart_settings};
StUsart usart{usart_params};

};  // namespace Stmf4
};  // namespace MM

namespace MM
{

Board board{.adc = MM::Stmf4::adc,
            .dma = MM::Stmf4::dma,
            .ir_led = MM::Stmf4::ir_led,
            .usart = MM::Stmf4::usart,
            .clk = MM::Stmf4::clk,
            .tx = MM::Stmf4::tx};

bool board_init()
{
    bool result = true;

    // Keep watchdog IRQ disabled during adc_test bring-up
    NVIC_DisableIRQ(WWDG_IRQn);
    NVIC_ClearPendingIRQ(WWDG_IRQn);

    // Init SYSCLK/HCLK and configure prescalers for APB1 and APB2
    result &= Stmf4::clk.init();
    result &= Stmf4::usart.set_clock_freq(Stmf4::clk.get_freq());

    // Init Periph Clks
    RCC->AHB1ENR |=
        (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_DMA2EN);
    RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | RCC_APB2ENR_USART1EN);

    // Init Periphs
    result &= Stmf4::ir_led.init();
    result &= Stmf4::phototrans.init();
    result &= Stmf4::tx.init();
    result &= Stmf4::dma.init();
    result &= Stmf4::adc.init();
    result &= Stmf4::usart.init();

    // Enable USART1 IRQ for RXNE interrupt handling.
    NVIC_SetPriority(USART1_IRQn, 1);
    NVIC_EnableIRQ(USART1_IRQn);

    return result;
}

Board& get_board()
{
    return board;
}

extern "C" void USART1_IRQHandler(void)
{
    // Check if data is available
    if (Stmf4::usart.get_addr()->SR & USART_SR_RXNE)
    {
        if (board.usart.receive(rx_byte))
        {
            // received 1 byte, echo it back
            std::span<const uint8_t> tx_span(&rx_byte, 1);
            board.usart.send(tx_span);
        }
    }
}

extern "C" void WWDG_IRQHandler(void)
{
    // Clear early wakeup interrupt flag and return.
    WWDG->SR = 0u;
}
};  // namespace MM
