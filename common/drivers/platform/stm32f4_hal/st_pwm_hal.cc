#include "st_pwm_hal.h"
#include <algorithm>
#include <cstring>
#include "reg_helpers.h"
#include "stm32f4xx_hal.h"

namespace MM
{
namespace Stmf4Hal
{

static constexpr uint8_t kArrVal = 99;  // match baremetal PWM period

HwPwmHal::HwPwmHal(const StPwmParams& params)
    : base_addr{params.base_addr},
      channel{params.channel},
      settings{params.settings},
      p_clk{params.p_clk},
      current_frequency{params.p_clk},
      current_duty_cycle{0}
{
}

TIM_HandleTypeDef& HwPwmHal::hal()
{
    return *reinterpret_cast<TIM_HandleTypeDef*>(hal_tim_handle_mem);
}

uint32_t HwPwmHal::hal_channel() const
{
    switch (channel)
    {
        case PwmChannel::CH1:
            return TIM_CHANNEL_1;
        case PwmChannel::CH2:
            return TIM_CHANNEL_2;
        case PwmChannel::CH3:
            return TIM_CHANNEL_3;
        case PwmChannel::CH4:
            return TIM_CHANNEL_4;
        default:
            return TIM_CHANNEL_1;
    }
}

bool HwPwmHal::init()
{
    if (base_addr == nullptr)
    {
        return false;
    }

    // Basic channel validity for 1..5 timers; for simplicity we allow CH1..CH4.
    // Caller/BSP must provide correct AF mapping.

    std::memset(hal_tim_handle_mem, 0, sizeof(hal_tim_handle_mem));
    auto& htim = hal();
    htim.Instance = base_addr;

    htim.Init.Prescaler = 0;
    htim.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim.Init.Period = kArrVal;
    htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim.Init.RepetitionCounter = 0;

    // Alignment
    switch (settings.mode)
    {
        case PwmMode::EDGE_ALIGNED:
            htim.Init.CounterMode = TIM_COUNTERMODE_UP;
            break;
        case PwmMode::CENTER_ALIGNED_DOWN:
            htim.Init.CounterMode = TIM_COUNTERMODE_DOWN;  // approximates
            break;
        case PwmMode::CENTER_ALIGNED_UP:
            htim.Init.CounterMode = TIM_COUNTERMODE_UP;
            break;
        case PwmMode::CENTER_ALIGNED_UP_DOWN:
            htim.Init.CounterMode = TIM_COUNTERMODE_CENTERALIGNED1;
            break;
        default:
            htim.Init.CounterMode = TIM_COUNTERMODE_UP;
            break;
    }

    // Direction
    if (settings.dir == PwmDir::DOWNCOUNTING)
    {
        htim.Init.CounterMode = (htim.Init.CounterMode == TIM_COUNTERMODE_UP)
                                    ? TIM_COUNTERMODE_DOWN
                                    : htim.Init.CounterMode;
    }

    if (HAL_TIM_PWM_Init(&htim) != HAL_OK)
    {
        return false;
    }

    TIM_OC_InitTypeDef sConfig{};
    sConfig.OCMode = TIM_OCMODE_PWM1;
    if (settings.output_mode == PwmOutputMode::PWM_MODE_2)
        sConfig.OCMode = TIM_OCMODE_PWM2;

    sConfig.Pulse = 0;
    sConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    sConfig.OCFastMode = TIM_OCFAST_DISABLE;

    if (HAL_TIM_PWM_ConfigChannel(&htim, &sConfig, hal_channel()) != HAL_OK)
    {
        return false;
    }

    if (HAL_TIM_PWM_Start(&htim, hal_channel()) != HAL_OK)
    {
        return false;
    }

    // Apply initial frequency
    return set_frequency(current_frequency);
}

bool HwPwmHal::set_frequency(uint32_t frequency)
{
    if (base_addr == nullptr)
        return false;

    if (frequency < 1)
        return false;

    auto& htim = hal();
    if (HAL_TIM_PWM_Stop(&htim, hal_channel()) != HAL_OK)
    {
        // still try to continue
    }

    const uint64_t arrp1 = static_cast<uint64_t>(kArrVal) + 1ULL;
    const uint64_t denom = static_cast<uint64_t>(frequency) * arrp1;

    uint64_t psc_val = (static_cast<uint64_t>(p_clk) + (denom / 2ULL)) / denom;
    if (psc_val < 1ULL)
        psc_val = 1ULL;

    if (psc_val > 65536ULL)
        return false;

    htim.Init.Prescaler = static_cast<uint32_t>(psc_val - 1ULL);
    if (HAL_TIM_PWM_Init(&htim) != HAL_OK)
        return false;

    return HAL_TIM_PWM_Start(&htim, hal_channel()) == HAL_OK;
}

bool HwPwmHal::set_duty_cycle(uint8_t duty_cycle)
{
    if (duty_cycle > 100)
        return false;

    auto& htim = hal();

    const uint32_t arrp1 = static_cast<uint32_t>(kArrVal) + 1U;
    const uint32_t ccr_val =
        (static_cast<uint32_t>(duty_cycle) * arrp1 + 50U) / 100U;

    __HAL_TIM_SET_COMPARE(&htim, hal_channel(), ccr_val);
    current_duty_cycle = duty_cycle;
    return true;
}

}  // namespace Stmf4Hal
}  // namespace MM
