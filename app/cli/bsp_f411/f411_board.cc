/**
 * @file f411_board.cc
 * @brief CLI app BSP — motors, encoders, IR sensors, LEDs, buttons.
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
static constexpr uint32_t kEncoderSampleUs{5'000};
static constexpr uint32_t kPwmFreqHz{323};
static constexpr uint32_t kIrTimerFreqHz{1'000'000};
static constexpr std::chrono::microseconds kIrTimerPeriod{100};
}  // namespace

namespace MM
{
namespace Stmf4
{

/* ── Clock ─────────────────────────────────────────────────────────────── */
HwClk clk{Configuration::SYSCLK_HSE_100MHZ};

/* ── Motor PWM GPIO (AF mode, TIM2) ───────────────────────────────────── */
StGpioSettings motor_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                                 GpioOspeed::LOW, GpioPupd::NO_PULL, 1};
// Left motor:  PA2 = TIM2_CH3, PA3 = TIM2_CH4
StGpioParams in1_left_params{2, GPIOA, motor_af_settings};
StGpioParams in2_left_params{3, GPIOA, motor_af_settings};
HwGpio in1_left{in1_left_params};
HwGpio in2_left{in2_left_params};
// Right motor: PA15 = TIM2_CH1, PB3 = TIM2_CH2
StGpioParams in1_right_params{15, GPIOA, motor_af_settings};
StGpioParams in2_right_params{3, GPIOB, motor_af_settings};
HwGpio in1_right{in1_right_params};
HwGpio in2_right{in2_right_params};

/* ── Encoder GPIO (AF mode, TIM3/TIM4) ────────────────────────────────── */
StGpioSettings enc_af_settings{GpioMode::AF, GpioOtype::PUSH_PULL,
                               GpioOspeed::LOW, GpioPupd::PULL_UP, 2};
// Left encoder:  PB6 = TIM4_CH1(B), PB7 = TIM4_CH2(A)
StGpioParams enc_left_ch1_params{6, GPIOB, enc_af_settings};
StGpioParams enc_left_ch2_params{7, GPIOB, enc_af_settings};
HwGpio enc_left_ch1{enc_left_ch1_params};
HwGpio enc_left_ch2{enc_left_ch2_params};
// Right encoder: PC6 = TIM3_CH1(A), PB5 = TIM3_CH2(B)
StGpioParams enc_right_ch1_params{6, GPIOC, enc_af_settings};
StGpioParams enc_right_ch2_params{5, GPIOB, enc_af_settings};
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

/* ── IR emitters (output GPIO) ────────────────────────────────────────── */
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

/* ── IR phototransistors (analog input GPIO) ─────────────────────────── */
StGpioSettings ir_pt_settings{GpioMode::ANALOG, GpioOtype::PUSH_PULL,
                              GpioOspeed::VERY_HIGH, GpioPupd::NO_PULL, 0};
StGpioParams ir_pt1_params{1, GPIOB, ir_pt_settings};  // PB1 = ADC1_IN9
StGpioParams ir_pt2_params{0, GPIOB, ir_pt_settings};  // PB0 = ADC1_IN8
StGpioParams ir_pt3_params{1, GPIOC, ir_pt_settings};  // PC1 = ADC1_IN11
StGpioParams ir_pt4_params{0, GPIOC, ir_pt_settings};  // PC0 = ADC1_IN10
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

/* ── IR timebase (TIM1, 100 µs period) ───────────────────────────────── */
StTimebaseParams timebase_params{TIM1};
HwTimebase timebase{timebase_params};

/* ── Delay timer (TIM5) ───────────────────────────────────────────────── */
StTimebaseParams delay_params{TIM5};
HwTimebase delay_timer{delay_params};

/* ── LEDs and buttons ─────────────────────────────────────────────────── */
StGpioSettings led_settings{GpioMode::GPOUT, GpioOtype::PUSH_PULL,
                            GpioOspeed::LOW, GpioPupd::NO_PULL, 0};
StGpioSettings button_settings{GpioMode::GPI, GpioOtype::PUSH_PULL,
                               GpioOspeed::LOW, GpioPupd::PULL_UP, 0};
StGpioParams led1_params{3, GPIOC, led_settings};          // PC3
StGpioParams led2_params{4, GPIOC, led_settings};          // PC4
StGpioParams led3_params{5, GPIOC, led_settings};          // PC5
StGpioParams search_bt_params{4, GPIOB, button_settings};  // PB4
StGpioParams zoom_bt_params{12, GPIOA, button_settings};   // PA12
HwGpio led1{led1_params};
HwGpio led2{led2_params};
HwGpio led3{led3_params};
HwGpio search_bt{search_bt_params};
HwGpio zoom_bt{zoom_bt_params};

}  // namespace Stmf4

/* ── MotionController ─────────────────────────────────────────────────── */
static constexpr Gains kDefaultGains{.kp = 0.000018f,
                                     .ki = 0.0f,
                                     .kd = 0.000018f};
MotionControllerParams mc_params{.motor_left = Stmf4::motor_left,
                                 .motor_right = Stmf4::motor_right,
                                 .encoder_left = Stmf4::encoder_left,
                                 .encoder_right = Stmf4::encoder_right,
                                 .gains = kDefaultGains,
                                 .encoder_sample_us = kEncoderSampleUs};
MotionController motion_controller{mc_params};

/* ── Board ────────────────────────────────────────────────────────────── */
Board board{.motion_controller = motion_controller,
            .ir_controller = Stmf4::ircontroller,
            .led1 = Stmf4::led1,
            .led2 = Stmf4::led2,
            .led3 = Stmf4::led3,
            .search_bt = Stmf4::search_bt,
            .zoom_bt = Stmf4::zoom_bt};

/* ── BSP init ─────────────────────────────────────────────────────────── */
bool board_init()
{
    bool ret = true;

    ret = ret && Stmf4::clk.init();
    const uint32_t hclk = Stmf4::clk.get_freq();
    const uint32_t tim_pclk =
        hclk / 2u;  // APB1 timers ×2 at default prescalers

    /* Enable peripheral bus clocks */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN |
                    RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_DMA2EN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN |
                    RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM5EN;
    RCC->APB2ENR |= RCC_APB2ENR_TIM1EN | RCC_APB2ENR_ADC1EN;

    /* Motor GPIO + PWM */
    ret = ret && Stmf4::in1_left.init();
    ret = ret && Stmf4::in2_left.init();
    ret = ret && Stmf4::in1_right.init();
    ret = ret && Stmf4::in2_right.init();
    ret = ret && Stmf4::pwm1_left.init();
    ret = ret && Stmf4::pwm2_left.init();
    ret = ret && Stmf4::pwm1_right.init();
    ret = ret && Stmf4::pwm2_right.init();
    ret =
        ret && Stmf4::pwm1_left.set_frequency(kPwmFreqHz);  // all 4 share TIM2
    ret = ret && Stmf4::motor_left.init();
    ret = ret && Stmf4::motor_right.init();

    /* Encoders */
    ret = ret && Stmf4::enc_left_ch1.init();
    ret = ret && Stmf4::enc_left_ch2.init();
    ret = ret && Stmf4::encoder_left.init();
    ret = ret && Stmf4::enc_right_ch1.init();
    ret = ret && Stmf4::enc_right_ch2.init();
    ret = ret && Stmf4::encoder_right.init();

    /* IR emitters + phototransistors */
    ret = ret && Stmf4::ir_emit1.init();
    ret = ret && Stmf4::ir_emit2.init();
    ret = ret && Stmf4::ir_emit3.init();
    ret = ret && Stmf4::ir_emit4.init();
    ret = ret && Stmf4::ir_pt1.init();
    ret = ret && Stmf4::ir_pt2.init();
    ret = ret && Stmf4::ir_pt3.init();
    ret = ret && Stmf4::ir_pt4.init();

    /* DMA + ADC */
    ret = ret && Stmf4::dma.init();
    ret = ret && Stmf4::adc.init();

    /* IR timebase: TIM1 at 100 µs period */
    ret = ret &&
          Stmf4::timebase.init(tim_pclk, kIrTimerFreqHz, kIrTimerPeriod, true);
    if (ret)
    {
        Stmf4::timebase.start();
    }

    /* Delay timer: TIM5 */
    ret = ret && Stmf4::delay_timer.init(50'000'000u, 1'000'000u,
                                         std::chrono::microseconds(4'294'967u));
    Stmf4::delay_timer.start();
    Utils::bind_timebase(Stmf4::delay_timer);

    /* LEDs + buttons */
    ret = ret && Stmf4::led1.init();
    ret = ret && Stmf4::led2.init();
    ret = ret && Stmf4::led3.init();
    ret = ret && Stmf4::search_bt.init();
    ret = ret && Stmf4::zoom_bt.init();

    Stmf4::led1.set(false);
    Stmf4::led2.set(false);
    Stmf4::led3.set(false);

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
