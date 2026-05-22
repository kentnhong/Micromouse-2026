#include <array>
#include "../board.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"

namespace MM
{
namespace Stmf4
{

// PA7 — IR LED emitter
StGpioSettings led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                            GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams led_params{7, GPIOA, led_settings};
HwGpio ir_led{led_params};

// PB1 — Phototransistor (ADC1 channel 9)
StGpioSettings pt_settings{GpioMode::ANALOG, GpioOtype::PUSH_PULL,
                           GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams pt_params{1, GPIOB, pt_settings};
HwGpio phototrans{pt_params};

// DMA2 stream 0 channel 0 — ADC1 peripheral to memory
StDmaSettings dma_settings{DmaChSel::CH0, DmaPriority::VERY_HIGH,
                           DmaWidth::HALF_WORD, DmaDataDir::PERIPH_TO_MEM};
StDmaParams dma_params{
    dma_settings, DMA2, DMA2_Stream0,
    static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&ADC1->DR))};
HwDma dma{dma_params};

// ADC1 — single channel 9, software trigger, no overrun interrupt
std::array<uint8_t, 1> adc_seq{9};
AdcChCycles ch9_cycles{.ch = 9, .cycles = AdcCycles::CYCLES_480};
std::array<AdcChCycles, 1> adc_ch_cycles{ch9_cycles};
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

}  // namespace Stmf4

Board board{.adc = Stmf4::adc, .dma = Stmf4::dma, .ir_led = Stmf4::ir_led};

bool board_init()
{
    bool result = true;

    result &= Stmf4::clk.init();

    RCC->AHB1ENR |=
        (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_DMA2EN);
    RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;

    result &= Stmf4::ir_led.init();
    result &= Stmf4::phototrans.init();
    result &= Stmf4::dma.init();
    result &= Stmf4::adc.init();

    return result;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
