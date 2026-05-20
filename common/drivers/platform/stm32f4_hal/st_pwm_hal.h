/**
 * @file st_pwm_hal.h
 * @brief STM32F4 HAL-based PWM driver header interface
 * @date 2026-05-20
 */

#pragma once
#include <cstdint>
#include "pwm.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"

namespace MM
{
namespace Stmf4Hal
{

enum class PwmMode : uint8_t
{
    EDGE_ALIGNED = 0,
    CENTER_ALIGNED_DOWN,
    CENTER_ALIGNED_UP,
    CENTER_ALIGNED_UP_DOWN
};

enum class PwmOutputMode : uint8_t
{
    PWM_MODE_1 = 6,
    PWM_MODE_2 = 7
};

enum class PwmDir : uint8_t
{
    UPCOUNTING = 0,
    DOWNCOUNTING
};

enum class PwmChannel : uint8_t
{
    CH1 = 1,
    CH2 = 2,
    CH3 = 3,
    CH4 = 4
};

struct StPwmSettings
{
    PwmMode mode;
    PwmOutputMode output_mode;
    PwmDir dir;
};

struct StPwmParams
{
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
    uint32_t p_clk;
};

class HwPwmHal : public Pwm
{
public:
    explicit HwPwmHal(const StPwmParams& params);

    bool init();

    bool set_frequency(uint32_t frequency) override;

    bool set_duty_cycle(uint8_t duty_cycle) override;

private:
    TIM_TypeDef* base_addr;
    PwmChannel channel;
    StPwmSettings settings;
    uint32_t p_clk;

    uint32_t current_frequency;
    uint8_t current_duty_cycle;

    // We store the HAL timer handle so we can use the HAL helper macros.
    // HAL expects the handle lifetime to be valid.
    // (stm32f4xx_hal.h is assumed available by the build for this platform.)
    TIM_HandleTypeDef htim_{};

    TIM_HandleTypeDef& hal();

    uint32_t hal_channel() const;
};

}  // namespace Stmf4Hal
}  // namespace MM
