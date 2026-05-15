#include "../board.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_gpio.h"
#include "st_sys_clk.h"
#include "st_timebase.h"
#include "st_usart.h"

// ADC IRQ will set this to true if there is overrun, then we can just handle it with board_recover()
volatile bool g_adc_ovr = false;

namespace
{
static constexpr uint32_t kTimerFreq{1'000'000};
static constexpr std::chrono::microseconds kTimerPeriod{100};
};  // namespace

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
                           AdcOverrunInt::OVRIE_EN,
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
// TODO: Change to HSE 24MHz (need sysclk hotfix)
HwClk clk{Configuration::HSI_16MHZ};

/* USART Initialization */
StUsartSettings usart_settings{UsartOversample::X8, UsartSampleMode::SINGLE};
StUsartParams usart_params{USART2, clk.get_freq(), 115200, usart_settings};
StUsart usart{usart_params};

}  // namespace Stmf4
}  // namespace MM

namespace MM
{
/* Non-hardware specific object creation and initialization */
Board board{.ir_controller = MM::Stmf4::ircontroller,
            .timebase = MM::Stmf4::timebase,
            .usart = MM::Stmf4::usart};

bool board_init()
{
    bool result = true;

    /* Init SYSCLK/HCLK and configure prescalers for APB1 and APB2 */
    result = result && Stmf4::clk.init();
    uint32_t hclk = Stmf4::clk.get_freq();

    /* Init Periph Clks */
    // Enable GPIO Bus Clocks
    RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                     RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA2EN);
    // Enable USART2 Bus Clock
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // Enable ADC1 and TIM1 Bus Clock
    RCC->APB2ENR |= (RCC_APB2ENR_ADC1EN | RCC_APB2ENR_TIM1EN);

    /* Init Periphs */
    // IR Emitter Inits
    result = result && Stmf4::led1.init();
    result = result && Stmf4::led2.init();
    result = result && Stmf4::led3.init();
    result = result && Stmf4::led4.init();

    // Phototransistor Inits
    result = result && Stmf4::pt1.init();
    result = result && Stmf4::pt2.init();
    result = result && Stmf4::pt3.init();
    result = result && Stmf4::pt4.init();

    // DMA, ADC, USART, and Timebase Inits
    result = result && Stmf4::dma.init();
    result = result && Stmf4::adc.init();
    result = result && Stmf4::usart.init();
    result =
        result && Stmf4::timebase.init(hclk, kTimerFreq, kTimerPeriod, true);

    /* Interrupt Enables */
    // Enable ADC1 interrupts in NVIC
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(ADC_IRQn, 0);

    // Enable TIM1 interrupt in NVIC
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0);

    // Enable USART2 interrupt in NVIC
    NVIC_EnableIRQ(USART2_IRQn);
    NVIC_SetPriority(USART2_IRQn, 1);

    return result;
}

void board_recover()
{
    // Stop TIM1 from firing during recovery
    NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);

    // Stop any in-progress ADC conversion before aborting DMA
    Stmf4::adc.stop();

    // Abort any in-progress DMA transfer
    Stmf4::dma.abort();

    // Clear ADC overrun flag and re-enable DMA requests
    Stmf4::adc.ovr_recover();
    Stmf4::adc.en_dma_req();

    // Turn off all IR emitters
    Stmf4::led1.set(0);
    Stmf4::led2.set(0);
    Stmf4::led3.set(0);
    Stmf4::led4.set(0);

    // Reset IR controller and all sensor state machines
    Stmf4::ircontroller.reset();

    // Clear fault flag
    g_adc_ovr = false;

    // Re-enable interrupts used by the IR pipeline
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

Board& get_board()
{
    return board;
}

extern "C" void ADC_IRQHandler()
{
    // Latch fault and disable ADC IRQ to prevent re-entry until board_recover()
    g_adc_ovr = true;
    NVIC_DisableIRQ(ADC_IRQn);
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

    // Handle ADC overrun recovery before running the IR state machine
    if (g_adc_ovr)
    {
        board_recover();
        return;
    }

    // IrController update state
    Stmf4::ircontroller.update();
}
}  // namespace MM