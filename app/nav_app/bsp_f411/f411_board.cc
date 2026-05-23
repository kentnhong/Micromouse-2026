/**
 * @file f411_board.cc
 * @brief Navigation app BSP — motors, encoders, IR sensors.
 * @author Bex Saw
 */

#include <array>
#include <cstdint>
#include "../board.h"
#include "drv8231.h"
#include "ircontroller.h"
#include "motioncontroller.h"
#include "st_adc.h"
#include "st_dma.h"
#include "st_encoder.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"
#include "st_timebase.h"

volatile bool g_adc_ovr = false;

namespace
{
static constexpr uint32_t kEncoderSampleUs{20'000};
static constexpr uint32_t kIrTimerFreqHz{1'000'000};
static constexpr std::chrono::microseconds kIrTimerPeriod{100};
}  // namespace

namespace MM
{
namespace Stmf4
{

/* ── Clock ─────────────────────────────────────────────────────────────── */
HwClk clk{Configuration::SYSCLK_HSE_100MHZ};

/* ── Motor PWM GPIO (AF, TIM2) ────────────────────────────────────────── */
StGpioSettings motor_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                  GpioOspeed::LOW, GpioPupd::NO_PULL, 1};
StGpioParams in1_left_params{2, GPIOA, motor_af_settings};   // PA2 TIM2_CH3
StGpioParams in2_left_params{3, GPIOA, motor_af_settings};   // PA3 TIM2_CH4
StGpioParams in1_right_params{15, GPIOA, motor_af_settings}; // PA15 TIM2_CH1
StGpioParams in2_right_params{3, GPIOB, motor_af_settings};  // PB3  TIM2_CH2
HwGpio in1_left{in1_left_params};
HwGpio in2_left{in2_left_params};
HwGpio in1_right{in1_right_params};
HwGpio in2_right{in2_right_params};

/* ── Encoder GPIO (AF, TIM3/TIM4) ────────────────────────────────────── */
StGpioSettings enc_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                GpioOspeed::LOW, GpioPupd::PULL_UP, 2};
StGpioParams enc_left_ch1_params{6, GPIOB, enc_af_settings};  // PB6 TIM4_CH1
StGpioParams enc_left_ch2_params{7, GPIOB, enc_af_settings};  // PB7 TIM4_CH2
StGpioParams enc_right_ch1_params{6, GPIOC, enc_af_settings}; // PC6 TIM3_CH1
StGpioParams enc_right_ch2_params{5, GPIOB, enc_af_settings}; // PB5 TIM3_CH2
HwGpio enc_left_ch1{enc_left_ch1_params};
HwGpio enc_left_ch2{enc_left_ch2_params};
HwGpio enc_right_ch1{enc_right_ch1_params};
HwGpio enc_right_ch2{enc_right_ch2_params};

/* ── PWM ──────────────────────────────────────────────────────────────── */
StPwmSettings pwm_settings{PwmMode::EDGE_ALIGNED, PwmOutputMode::PWM_MODE_1,
                            PwmDir::UPCOUNTING};
StPwmParams pwm1_left_params{TIM2, PwmChannel::CH3, pwm_settings, 100'000'000};
StPwmParams pwm2_left_params{TIM2, PwmChannel::CH4, pwm_settings, 100'000'000};
StPwmParams pwm1_right_params{TIM2, PwmChannel::CH1, pwm_settings, 100'000'000};
StPwmParams pwm2_right_params{TIM2, PwmChannel::CH2, pwm_settings, 100'000'000};
HwPwm pwm1_left{pwm1_left_params};
HwPwm pwm2_left{pwm2_left_params};
HwPwm pwm1_right{pwm1_right_params};
HwPwm pwm2_right{pwm2_right_params};

/* ── Encoders ─────────────────────────────────────────────────────────── */
StEncoderSettings enc_settings{.mode = EncMode::MODE_3,
                                .channel = EncChannel::BOTH,
                                .polarity = EncInputPolarity::RISING,
                                .slave_mode = EncSlaveMode::DISABLED,
                                .invert_direction = false};
StEncoderParams encoder_left_params{TIM4, enc_settings};
StEncoderParams encoder_right_params{TIM3, enc_settings};
HwEncoder encoder_left{encoder_left_params};
HwEncoder encoder_right{encoder_right_params};

/* ── Motor drivers ────────────────────────────────────────────────────── */
Drv8231 motor_left{pwm1_left, pwm2_left};
Drv8231 motor_right{pwm1_right, pwm2_right};

/* ── IR emitters ──────────────────────────────────────────────────────── */
StGpioSettings ir_emit_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                                 GpioOspeed::VERY_HIGH, GpioPupd::PULL_DOWN, 0};
StGpioParams ir_emit1_params{7, GPIOA, ir_emit_settings};  // PA7 → LEFT
StGpioParams ir_emit2_params{6, GPIOA, ir_emit_settings};  // PA6 → FRONT_LEFT
StGpioParams ir_emit3_params{4, GPIOA, ir_emit_settings};  // PA4 → FRONT_RIGHT
StGpioParams ir_emit4_params{5, GPIOA, ir_emit_settings};  // PA5 → RIGHT
HwGpio ir_emit1{ir_emit1_params};
HwGpio ir_emit2{ir_emit2_params};
HwGpio ir_emit3{ir_emit3_params};
HwGpio ir_emit4{ir_emit4_params};

/* ── IR phototransistors ──────────────────────────────────────────────── */
StGpioSettings ir_pt_settings{GpioMode::ANALOG, GpioOtype::PUSH_PULL,
                               GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams ir_pt1_params{1, GPIOB, ir_pt_settings};  // PB1 ADC1_IN9
StGpioParams ir_pt2_params{0, GPIOB, ir_pt_settings};  // PB0 ADC1_IN8
StGpioParams ir_pt3_params{1, GPIOC, ir_pt_settings};  // PC1 ADC1_IN11
StGpioParams ir_pt4_params{0, GPIOC, ir_pt_settings};  // PC0 ADC1_IN10
HwGpio ir_pt1{ir_pt1_params};
HwGpio ir_pt2{ir_pt2_params};
HwGpio ir_pt3{ir_pt3_params};
HwGpio ir_pt4{ir_pt4_params};

/* ── DMA ──────────────────────────────────────────────────────────────── */
StDmaSettings dma_settings{DmaChSel::CH0, DmaPriority::VERY_HIGH,
                            DmaWidth::HALF_WORD, DmaDataDir::PERIPH_TO_MEM};
StDmaParams dma_params{
    dma_settings, DMA2, DMA2_Stream0,
    static_cast<uint32_t>(reinterpret_cast<std::uintptr_t>(&ADC1->DR))};
HwDma dma{dma_params};

/* ── ADC ──────────────────────────────────────────────────────────────── */
std::array<uint8_t, 1> adc_seq{9};
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

/* ── IR sensors ───────────────────────────────────────────────────────── */
IrParams ir1_params{adc, dma, ir_emit1, 9};   // LEFT
IrParams ir2_params{adc, dma, ir_emit2, 8};   // FRONT_LEFT
IrParams ir3_params{adc, dma, ir_emit3, 11};  // FRONT_RIGHT
IrParams ir4_params{adc, dma, ir_emit4, 10};  // RIGHT
IrSensor ir1{ir1_params};
IrSensor ir2{ir2_params};
IrSensor ir3{ir3_params};
IrSensor ir4{ir4_params};

/* ── IR controller ────────────────────────────────────────────────────── */
std::array<IrSensor*, 4> ir_sequence{&ir1, &ir2, &ir3, &ir4};
IrValues ir_vals{};
IrControllerParams ircontroller_params{ir_sequence, ir_vals};
IrController ircontroller{ircontroller_params};

/* ── IR timebase (TIM1, 100 µs) ──────────────────────────────────────── */
StTimebaseParams timebase_params{TIM1};
HwTimebase timebase{timebase_params};

/* ── Delay timer (TIM5) ───────────────────────────────────────────────── */
StTimebaseParams delay_params{TIM5};
HwTimebase delay_timer{delay_params};

}  // namespace Stmf4

/* ── MotionController ─────────────────────────────────────────────────── */
static constexpr Gains kDefaultGains{.kp = 1.5f, .ki = 0.3f, .kd = 0.05f};
MotionControllerParams mc_params{.motor_left = Stmf4::motor_left,
                                  .motor_right = Stmf4::motor_right,
                                  .encoder_left = Stmf4::encoder_left,
                                  .encoder_right = Stmf4::encoder_right,
                                  .gains = kDefaultGains,
                                  .encoder_sample_us = kEncoderSampleUs};
MotionController motion_controller{mc_params};

/* ── Board ────────────────────────────────────────────────────────────── */
Board board{.ir_controller = Stmf4::ircontroller,
            .motion_controller = motion_controller};

/* ── BSP init ─────────────────────────────────────────────────────────── */
bool bsp_init()
{
    bool ret = true;

    ret = ret && Stmf4::clk.init();
    const uint32_t hclk = Stmf4::clk.get_freq();
    const uint32_t tim_pclk = hclk / 2u;

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN |
                    RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_ADC1EN;

    /* Motors */
    ret = ret && Stmf4::in1_left.init();
    ret = ret && Stmf4::in2_left.init();
    ret = ret && Stmf4::in1_right.init();
    ret = ret && Stmf4::in2_right.init();
    ret = ret && Stmf4::pwm1_left.init();
    ret = ret && Stmf4::pwm2_left.init();
    ret = ret && Stmf4::pwm1_right.init();
    ret = ret && Stmf4::pwm2_right.init();
    ret = ret && Stmf4::motor_left.init();
    ret = ret && Stmf4::motor_right.init();

    /* Encoders */
    ret = ret && Stmf4::enc_left_ch1.init();
    ret = ret && Stmf4::enc_left_ch2.init();
    ret = ret && Stmf4::encoder_left.init();
    ret = ret && Stmf4::enc_right_ch1.init();
    ret = ret && Stmf4::enc_right_ch2.init();
    ret = ret && Stmf4::encoder_right.init();

    /* IR hardware */
    ret = ret && Stmf4::ir_emit1.init();
    ret = ret && Stmf4::ir_emit2.init();
    ret = ret && Stmf4::ir_emit3.init();
    ret = ret && Stmf4::ir_emit4.init();
    ret = ret && Stmf4::ir_pt1.init();
    ret = ret && Stmf4::ir_pt2.init();
    ret = ret && Stmf4::ir_pt3.init();
    ret = ret && Stmf4::ir_pt4.init();
    ret = ret && Stmf4::dma.init();
    ret = ret && Stmf4::adc.init();

    /* IR timebase: TIM1 at 100 µs */
    ret = ret && Stmf4::timebase.init(tim_pclk, kIrTimerFreqHz, kIrTimerPeriod, true);
    if (ret)
    {
        Stmf4::timebase.start();
    }

    /* Delay timer: TIM5 */
    ret = ret && Stmf4::delay_timer.init(50'000'000u, 1'000'000u,
                                          std::chrono::microseconds(4'294'967u));
    Stmf4::delay_timer.start();
    Utils::bind_timebase(Stmf4::delay_timer);

    /* Interrupts */
    NVIC_SetPriority(ADC_IRQn, 0);
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_SetPriority(TIM1_UP_TIM10_IRQn, 0);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);

    return ret;
}

Board& get_board()
{
    return board;
}

/* ── ISR handlers ─────────────────────────────────────────────────────── */

void board_recover()
{
    NVIC_DisableIRQ(TIM1_UP_TIM10_IRQn);
    Stmf4::adc.stop();
    Stmf4::dma.abort();
    Stmf4::adc.ovr_recover();
    Stmf4::adc.en_dma_req();
    Stmf4::ir_emit1.set(0);
    Stmf4::ir_emit2.set(0);
    Stmf4::ir_emit3.set(0);
    Stmf4::ir_emit4.set(0);
    Stmf4::ircontroller.reset();
    g_adc_ovr = false;
    NVIC_EnableIRQ(ADC_IRQn);
    NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
}

extern "C" void ADC_IRQHandler()
{
    g_adc_ovr = true;
    NVIC_DisableIRQ(ADC_IRQn);
}

extern "C" void TIM1_UP_TIM10_IRQHandler()
{
    TIM1->SR &= ~TIM_SR_UIF;
    if (g_adc_ovr)
    {
        board_recover();
        return;
    }
    Stmf4::ircontroller.update();
}

}  // namespace MM
