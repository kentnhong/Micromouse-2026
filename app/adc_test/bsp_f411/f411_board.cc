#include "board.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"

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
StDmaParams dma_params{dma_settings, DMA2, DMA2_Stream0};
HwDma dma{dma_params};

StAdcSettings adc_settings{
    AdcResolution::TWELVE_BIT, AdcClkPrescaler::PCLK2_DIV_2,
    AdcTriggerSource::SOFTWARE, AdcOverrunInt::OVRIE_DIS, AdcDma::DMA_ENABLE};
StAdcParams adc_params{adc_settings, ADC1, ADC1_COMMON};
HwAdc adc{adc_params};
};  // namespace Stmf4
};  // namespace MM

namespace MM
{

Board board{.adc = MM::Stmf4::adc,
            .dma = MM::Stmf4::dma,
            .ir_led = MM::Stmf4::ir_led};
Stmf4::HwClk clk{Stmf4::Configuration::HSI_16MHZ};

bool board_init()
{
    bool result = true;

    // Init SYSCLK/HCLK and configure prescalers for APB1 and APB2
    result &= clk.init();

    // Init Periph Clks
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_DMA2EN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    // Init Periphs
    result &= Stmf4::ir_led.init();
    result &= Stmf4::dma.init();
    result &= Stmf4::adc.init();

    return result;
}

bool board_recover()
{
    __disable_irq();
    g_adc_ovr = false;
    __enable_irq();
    /*
     * stop timer-triggered conversions
     * stop or disable the ADC/DMA path
     * clear the ADC overrun flag
     * reset your IR sequencing state
     * reset or re-arm DMA
     * turn all LEDs off
     * restart from sensor 0 / ambient sample 1
     */
    return true;
}

Board& get_board()
{
    return board;
}

};  // namespace MM

// Interrupt Handler for ADC overrun
extern "C" void ADC_IRQHandler()
{
    // Set global interrupt flag to true for ADC overrun
    g_adc_ovr = true;
}