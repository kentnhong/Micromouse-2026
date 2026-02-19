#include "st_pwm.h"
#include <cstdint>
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4
{
// Constants values for PWM calculations and register bit widths
static constexpr uint32_t kPclkFreq = 16000000;  // 16 MHz timer clock
static constexpr uint8_t kTimCcmrxOcxmBitWidth = 3;
static constexpr uint8_t kTimCr1CmsBitWidth = 2;
static constexpr uint8_t kTimCr1DirBitWidth = 1;
static constexpr uint8_t kArrVal =
    99;  // counts 0..99 => 100 ticks per PWM period
static constexpr uint32_t kMaxFreqEdgeAligned = kPclkFreq / (kArrVal + 1);
static constexpr uint32_t kMaxFreqCenterAligned =
    kPclkFreq / (2 * (kArrVal + 1));

// Helper functions to check timer type for channel validity and mode rules
static inline bool is_timer_1_to_5(TIM_TypeDef* t)
{
    return (t == TIM1 || t == TIM2 || t == TIM3 || t == TIM4 || t == TIM5);
}
static inline bool is_timer_9_to_11(TIM_TypeDef* t)
{
    return (t == TIM9 || t == TIM10 || t == TIM11);
}

HwPwm::HwPwm(const StPwmParams& params)
    : base_addr{params.base_addr},
      channel{params.channel},
      settings{params.settings},
      current_frequency{kPclkFreq},
      current_duty_cycle{0}
{
}

bool HwPwm::init()
{
    if (base_addr == nullptr)
    {
        return false;
    }

    // Channel validity by timer
    if (is_timer_9_to_11(base_addr))
    {
        // TIM9 has CH1..CH2, TIM10 has CH1, TIM11 has CH1
        if (base_addr == TIM9)
        {
            if (channel != PwmChannel::CH1 && channel != PwmChannel::CH2)
                return false;
        }
        else
        {
            if (channel != PwmChannel::CH1)
                return false;
        }
    }
    else if (is_timer_1_to_5(base_addr))
    {
        if (channel != PwmChannel::CH1 && channel != PwmChannel::CH2 &&
            channel != PwmChannel::CH3 && channel != PwmChannel::CH4)
            return false;
    }
    else
    {
        return false;
    }

    // Mode validity (per your rule)
    if (is_timer_9_to_11(base_addr))
    {
        if (settings.mode != PwmMode::EDGE_ALIGNED ||
            settings.dir != PwmDir::UPCOUNTING)
        {
            return false;
        }
    }

    // Configure PWM output channel registers
    switch (channel)
    {
        case PwmChannel::CH1:
            // CC1 as output
            base_addr->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR1_OC1M_Pos, kTimCcmrxOcxmBitWidth);
            base_addr->CCMR1 |= TIM_CCMR1_OC1PE;
            base_addr->CCR1 = 0;
            base_addr->CCER |= TIM_CCER_CC1E;
            break;
        case PwmChannel::CH2:
            base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR1_OC2M_Pos, kTimCcmrxOcxmBitWidth);
            base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            base_addr->CCR2 = 0;
            base_addr->CCER |= TIM_CCER_CC2E;
            break;
        case PwmChannel::CH3:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR2_OC3M_Pos, kTimCcmrxOcxmBitWidth);
            base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            base_addr->CCR3 = 0;
            base_addr->CCER |= TIM_CCER_CC3E;
            break;
        case PwmChannel::CH4:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode),
                   TIM_CCMR2_OC4M_Pos, kTimCcmrxOcxmBitWidth);
            base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            base_addr->CCR4 = 0;
            base_addr->CCER |= TIM_CCER_CC4E;
            break;
        default:
            return false;
    }

    // TIM1 needs MOE for outputs
    if (base_addr == TIM1)
    {
        base_addr->BDTR |= TIM_BDTR_MOE;
    }

    // Alignment mode
    SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.mode),
           TIM_CR1_CMS_Pos, kTimCr1CmsBitWidth);

    // Direction
    SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.dir),
           TIM_CR1_DIR_Pos, kTimCr1DirBitWidth);

    // Fixed ARR
    base_addr->ARR = kArrVal;

    // Buffer ARR updates
    base_addr->CR1 |= TIM_CR1_ARPE;

    // Reset counter + force update to latch preloads
    base_addr->CNT = 0;
    base_addr->EGR |= TIM_EGR_UG;

    // Enable counter
    base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}

bool HwPwm::set_frequency(uint32_t frequency)
{
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if (frequency < 1)
    {
        return false;
    }

    if ((settings.mode == PwmMode::EDGE_ALIGNED) &&
        (frequency > kMaxFreqEdgeAligned))
    {
        return false;
    }

    if ((settings.mode != PwmMode::EDGE_ALIGNED) &&
        (frequency > kMaxFreqCenterAligned))
    {
        return false;
    }

    // denom can exceed 2^32 when frequency is large
    // PSC+1 = round(pclk / (frequency * (ARR+1))) for edge-aligned
    const uint64_t arrp1 = static_cast<uint64_t>(kArrVal) + 1ULL;
    const uint64_t denom = static_cast<uint64_t>(frequency) * arrp1;

    // Rounded division: psc_val = round(pclk / denom)
    uint64_t psc_val =
        (static_cast<uint64_t>(kPclkFreq) + (denom / 2ULL)) / denom;

    // Center-aligned: same PSC/ARR → need half PSC+1
    if (settings.mode != PwmMode::EDGE_ALIGNED)
    {
        // rounded divide-by-2
        psc_val = (psc_val + 1ULL) / 2ULL;
    }

    // Valid PSC register is 0..65535 => psc_val (PSC+1) is 1..65536
    if (psc_val < 1ULL || psc_val > 65536ULL)
    {
        return false;
    }

    base_addr->PSC = static_cast<uint32_t>(psc_val - 1ULL);
    current_frequency = frequency;

    // force update so PSC takes effect immediately (safe/typical)
    base_addr->EGR |= TIM_EGR_UG;

    return true;
}

bool HwPwm::set_duty_cycle(uint8_t duty_cycle)
{
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if (duty_cycle > 100)
    {
        return false;
    }

    // CCR (Capture/Compare Register value) = round(duty% * (ARR+1) / 100)
    const uint32_t arrp1 = static_cast<uint32_t>(kArrVal) + 1U;
    uint32_t ccr_val = (static_cast<uint32_t>(duty_cycle) * arrp1 + 50U) / 100U;

    switch (channel)
    {
        case PwmChannel::CH1:
            base_addr->CCR1 = ccr_val;
            break;
        case PwmChannel::CH2:
            base_addr->CCR2 = ccr_val;
            break;
        case PwmChannel::CH3:
            base_addr->CCR3 = ccr_val;
            break;
        case PwmChannel::CH4:
            base_addr->CCR4 = ccr_val;
            break;
        default:
            return false;
    }

    current_duty_cycle = duty_cycle;

    return true;
}

}  // namespace Stmf4
}  // namespace MM
