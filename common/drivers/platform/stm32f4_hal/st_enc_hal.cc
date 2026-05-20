
#include "st_enc_hal.h"
#include <cstring>
#include "stm32f4xx_hal.h"

namespace MM
{
namespace Stmf4Hal
{

HwEncoderHal::HwEncoderHal(const StEncoderParams& params)
    : base_addr{params.base_addr},
      channel{params.settings.channel},
      settings{params.settings},
      current_ticks{0}
{
}

TIM_HandleTypeDef& HwEncoderHal::hal()
{
    return *reinterpret_cast<TIM_HandleTypeDef*>(hal_tim_handle_mem);
}

int32_t HwEncoderHal::sign_extend_cnt() const
{
    // STM32F4 TIM1..5 are 16-bit, others vary.
    // Conservative: sign extend 16-bit.
    uint32_t raw_cnt = base_addr->CNT;
    return static_cast<int32_t>(static_cast<int16_t>(raw_cnt));
}

void HwEncoderHal::configure_encoder_mode()
{
    auto& htim = hal();

    // Basic init: encoder via HAL_TIM_Encoder_Init.
    TIM_Encoder_InitTypeDef sConfig{};
    sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
    sConfig.IC1Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC2Polarity = TIM_ICPOLARITY_RISING;
    sConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    sConfig.IC1Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC2Prescaler = TIM_ICPSC_DIV1;
    sConfig.IC1Filter = 0;
    sConfig.IC2Filter = 0;

    // Map settings.mode roughly to HAL options.
    switch (settings.mode)
    {
        case EncMode::MODE_1:
            sConfig.EncoderMode = TIM_ENCODERMODE_TI1;
            break;
        case EncMode::MODE_2:
            sConfig.EncoderMode = TIM_ENCODERMODE_TI2;
            break;
        case EncMode::MODE_3:
        default:
            sConfig.EncoderMode = TIM_ENCODERMODE_TI12;
            break;
    }

    // Polarity (only falling is explicitly supported here; BOTH is mapped to rising)
    if (settings.polarity == EncInputPolarity::FALLING)
    {
        sConfig.IC1Polarity = TIM_ICPOLARITY_FALLING;
        sConfig.IC2Polarity = TIM_ICPOLARITY_FALLING;
    }

    if (HAL_TIM_Encoder_Init(&htim, &sConfig) != HAL_OK)
    {
    }
}

bool HwEncoderHal::init()
{
    if (base_addr == nullptr)
        return false;

    std::memset(hal_tim_handle_mem, 0, sizeof(hal_tim_handle_mem));
    auto& htim = hal();

    htim.Instance = base_addr;
    htim.Init.Prescaler = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = 0xFFFF;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.RepetitionCounter = 0;

    // init base
    if (HAL_TIM_Encoder_Init(&htim, nullptr) != HAL_OK)
    {
    }

    configure_encoder_mode();

    if (HAL_TIM_Encoder_Start(&htim, TIM_CHANNEL_ALL) != HAL_OK)
        return false;

    // reset counter
    __HAL_TIM_SET_COUNTER(&htim, 0);
    current_ticks = 0;

    return true;
}

int32_t HwEncoderHal::get_ticks()
{
    current_ticks = sign_extend_cnt();
    return current_ticks;
}

bool HwEncoderHal::reset_ticks()
{
    __HAL_TIM_SET_COUNTER(&hal(), 0);
    current_ticks = 0;
    return true;
}

bool HwEncoderHal::init_cycle_counter()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    volatile uint32_t* dwt_lar = (volatile uint32_t*)0xE0001FB0;
    *dwt_lar = 0xC5ACCE55;

    DWT->CYCCNT = 0;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0;
}

uint32_t HwEncoderHal::get_time_cycles()
{
    return DWT->CYCCNT;
}

uint32_t HwEncoderHal::cycles_per_us()
{
    // keep consistent with baremetal assumptions (16 MHz)
    return 16u;
}

EncoderStats HwEncoderHal::measure_encoder_stats(uint32_t sample_cycles,
                                                 float ticks_per_output_rev,
                                                 float cm_per_tick)
{
    int32_t start_ticks = get_ticks();
    uint32_t start_time = get_time_cycles();

    while ((get_time_cycles() - start_time) < sample_cycles)
    {
    }

    uint32_t end_time = get_time_cycles();
    uint32_t delta_cycles = end_time - start_time;

    uint32_t cycles_per_microsecond = cycles_per_us();

    if (delta_cycles == 0 || cycles_per_microsecond == 0 ||
        ticks_per_output_rev == 0.0f)
    {
        return {0.0f, 0.0f, 0.0f, 0.0f, 0};
    }

    int32_t end_ticks = get_ticks();
    int32_t delta_ticks = end_ticks - start_ticks;

    float delta_time_us = static_cast<float>(delta_cycles) /
                          static_cast<float>(cycles_per_microsecond);
    float delta_time_sec = delta_time_us / 1'000'000.0f;

    float ticks_per_sec = static_cast<float>(delta_ticks) / delta_time_sec;
    float rpm = (60.0f / ticks_per_output_rev) * ticks_per_sec;
    float velocity_cm_per_sec = cm_per_tick * ticks_per_sec;
    float distance_cm = cm_per_tick * static_cast<float>(delta_ticks);

    return {rpm, velocity_cm_per_sec, distance_cm, delta_time_sec, delta_ticks};
}

}  // namespace Stmf4Hal
}  // namespace MM
