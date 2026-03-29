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
                               GpioOspeed::MEDIUM, GpioPupd::NO_PULL, 0};
StGpioParams ir_led_params{4, GPIOA, ir_led_settings};
HwGpio ir_led{ir_led_params};

StDmaSettings dma_settings{DmaChSel::CH0, DmaPriority::VERY_HIGH,
                           DmaWidth::HALF_WORD, DmaDataDir::PERIPH_TO_MEM};
StDmaParams dma_params{
    dma_settings, DMA2, DMA2_Stream0,
    static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&ADC1->DR))};
HwDma dma{dma_params};

std::array<uint8_t, 1> adc_seq{8};
AdcChCycles ch_cycles{.ch = 8, .cycles = AdcCycles::CYCLES_3};
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

HwClk clk{Configuration::HSI_16MHZ};

StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};
StUsartParams usart_params{USART2, clk.get_freq(), 115200, usart_settings};
StUsart usart{usart_params};

};  // namespace Stmf4
};  // namespace MM

namespace MM
{

Board board{.adc = MM::Stmf4::adc,
            .dma = MM::Stmf4::dma,
            .ir_led = MM::Stmf4::ir_led,
            .usart = MM::Stmf4::usart,
            .clk = MM::Stmf4::clk};

bool board_init()
{
    bool result = true;

    // Init SYSCLK/HCLK and configure prescalers for APB1 and APB2
    result &= Stmf4::clk.init();

    // Init Periph Clks
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_DMA2EN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Init Periphs
    result &= Stmf4::ir_led.init();
    result &= Stmf4::dma.init();
    result &= Stmf4::adc.init();
    result &= Stmf4::usart.init();

    return result;
}

bool board_recover()
{
    // Could use atomic instead of disabling interrupts
    __disable_irq();
    g_adc_ovr = false;
    /*
     * stop timer-triggered conversions
     * stop or disable the ADC/DMA path
     * clear the ADC overrun flag
     * reset your IR sequencing state
     * reset or re-arm DMA
     * turn all LEDs off
     * restart from sensor 0 / ambient sample 1
     */
    __enable_irq();
    return true;
}

Board& get_board()
{
    return board;
}

// Interrupt Handler for ADC overrun
extern "C" void ADC_IRQHandler()
{
    // Set global interrupt flag to true for ADC overrun
    g_adc_ovr = true;
}

extern "C" void USART2_IRQHandler(void)
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
};  // namespace MM
