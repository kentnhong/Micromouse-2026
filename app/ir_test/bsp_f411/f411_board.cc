#include "../board.h"
#include "ircontroller.h"
#include "irsensor.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_timebase.h"
#include "st_usart.h"

namespace MM
{
namespace Stmf4
{
/* Emitters Initialization */
StGpioSettings led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                            GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams led1_params{7, GPIOA, led_settings};
StGpioParams led2_params{6, GPIOA, led_settings};
StGpioParams led3_params{4, GPIOA, led_settings};
StGpioParams led4_params{5, GPIOA, led_settings};
HwGpio led1{led1_params};
HwGpio led2{led2_params};
HwGpio led3{led3_params};
HwGpio led4{led4_params};

/* Phototransistors Initialization */
StGpioSettings pt_settings{GpioMode::ANALOG, GpioOtype::PUSH_PULL,
                           GpioOspeed::VERY_HIGH};
StGpioParams pt1_params{1, GPIOB, pt_settings};
StGpioParams pt2_params{0, GPIOB, pt_settings};
StGpioParams pt3_params{1, GPIOC, pt_settings};
StGpioParams pt4_params{0, GPIOC, pt_settings};
HwGpio pt1{pt1_params};
HwGpio pt2{pt2_params};
HwGpio pt3{pt3_params};
HwGpio pt4{pt4_params};

/* DMA Intialization */
StDmaSettings dma_settings{DmaChSel::CH0, DmaPriority::VERY_HIGH,
                           DmaWidth::HALF_WORD, DmaDataDir::PERIPH_TO_MEM};
StDmaParams dma_params{
    dma_settings, DMA2, DMA2_Stream0,
    static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&ADC1->DR))};
HwDma dma{dma_params};

/* ADC Initialization */
std::array<uint8_t, 4> adc_seq{9, 8, 11, 10};
AdcChCycles ch9_cycles{9, AdcCycles::CYCLES_144};
AdcChCycles ch8_cycles{8, AdcCycles::CYCLES_144};
AdcChCycles ch11_cycles{11, AdcCycles::CYCLES_144};
AdcChCycles ch10_cycles{10, AdcCycles::CYCLES_144};
std::array<AdcChCycles, 4> adc_ch_cycles{ch9_cycles, ch8_cycles, ch11_cycles,
                                         ch10_cycles};
StAdcSettings adc_settings{AdcResolution::TWELVE_BIT,
                           AdcClkPrescaler::PCLK2_DIV_2,
                           AdcTriggerSource::SOFTWARE,
                           AdcOverrunInt::OVRIE_DIS,
                           AdcDma::DMA_ENABLE,
                           adc_seq,
                           adc_ch_cycles};
StAdcParams adc_params{adc_settings, ADC1, ADC1_COMMON};
HwAdc adc{adc_params};

/* IR Sensors Initialization */
IrParams ir1_params{adc, dma, led1};
IrParams ir2_params{adc, dma, led2};
IrParams ir3_params{adc, dma, led3};
IrParams ir4_params{adc, dma, led4};
IrSensor ir1{ir1_params};
IrSensor ir2{ir2_params};
IrSensor ir3{ir3_params};
IrSensor ir4{ir4_params};

/* IR Controller Initialization */
std::array<IrSensor*, 4> ir_sequence{&ir1, &ir2, &ir3, &ir4};
IrValues ir_vals{};
IrControllerParams ircontroller_params{ir_sequence, ir_vals};
IrController ircontroller{ircontroller_params};

/* Timebase Initialization */
StTimebaseParams timebase_params{TIM1};
HwTimebase timebase{timebase_params};

/* Sysclk Initialization */
HwClk clk{Configuration::HSI_16MHZ};

/* USART Initialization */
StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};
StUsartParams usart_params{USART2, clk.get_freq(), 115200, usart_settings};
StUsart usart{usart_params};

}  // namespace Stmf4
}  // namespace MM

namespace MM
{
// Non-hardware specific object creation and initialization
Board board{.ir_controller = MM::Stmf4::ircontroller,
            .timebase = MM::Stmf4::timebase,
            .usart = MM::Stmf4::usart};

bool board_init()
{
    // TODO: Finish board init, also implement ADC overrun and TIM1_IRQHandler
    return true;
}

Board& get_board()
{
    return board;
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

extern "C" void TIM1_IRQHandler()
{
    // Clear the update interrupt flag
    TIM1->SR &= ~TIM_SR_UIF;

    // IrController update goes here
}
}  // namespace MM