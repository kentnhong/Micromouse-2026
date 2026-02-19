#include "st_pwm.h"
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4
{
// Constants values for PWM calculations and register bit widths
static constexpr uint32_t pclk_freq = 16000000;  // 16 MHz timer clock
static constexpr uint8_t TIM_CCMRx_OCxM_BitWidth = 3;
static constexpr uint8_t TIM_CR1_CMS_BitWidth = 2;
static constexpr uint8_t TIM_CR1_DIR_BitWidth = 1;
static constexpr uint8_t ARR_VAL =
    99;  // counts 0..99 => 100 ticks per PWM period
static constexpr uint32_t MAX_FREQ_EDGE_ALIGNED = pclk_freq / (ARR_VAL + 1);
static constexpr uint32_t MAX_FREQ_CENTER_ALIGNED =
    pclk_freq / (2 * (ARR_VAL + 1));

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
    : _base_addr{params.base_addr},
      _channel{params.channel},
      _settings{params.settings},
      _currentFrequency{pclk_freq},
      _currentDutyCycle{0}
{
}

bool HwPwm::init()
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Channel validity by timer
    if (is_timer_9_to_11(_base_addr))
    {
        // TIM9 has CH1..CH2, TIM10 has CH1, TIM11 has CH1
        if (_base_addr == TIM9)
        {
            if (_channel < 1 || _channel > 2)
                return false;
        }
        else
        {
            if (_channel != 1)
                return false;
        }
    }
    else if (is_timer_1_to_5(_base_addr))
    {
        if (_channel < 1 || _channel > 4)
            return false;
    }
    else
    {
        return false;
    }

    // Mode validity (per your rule)
    if (is_timer_9_to_11(_base_addr))
    {
        if (_settings.mode != PwmMode::EDGE_ALIGNED ||
            _settings.dir != PwmDir::UPCOUNTING)
        {
            return false;
        }
    }

    // Configure PWM output channel registers
    switch (_channel)
    {
        case 1:
            // CC1 as output
            _base_addr->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;

            // Output compare mode (PWM1/PWM2 etc.)
            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.outputMode),
                   TIM_CCMR1_OC1M_Pos, TIM_CCMRx_OCxM_BitWidth);

            // Preload enable for CCR1
            _base_addr->CCMR1 |= TIM_CCMR1_OC1PE;

            // Duty init
            _base_addr->CCR1 = 0;

            // Enable CH1 output
            _base_addr->CCER |= TIM_CCER_CC1E;
            break;

        case 2:
            _base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;

            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.outputMode),
                   TIM_CCMR1_OC2M_Pos, TIM_CCMRx_OCxM_BitWidth);

            _base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            _base_addr->CCR2 = 0;
            _base_addr->CCER |= TIM_CCER_CC2E;
            break;

        case 3:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;

            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.outputMode),
                   TIM_CCMR2_OC3M_Pos, TIM_CCMRx_OCxM_BitWidth);

            _base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            _base_addr->CCR3 = 0;
            _base_addr->CCER |= TIM_CCER_CC3E;
            break;

        case 4:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;

            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.outputMode),
                   TIM_CCMR2_OC4M_Pos, TIM_CCMRx_OCxM_BitWidth);

            _base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            _base_addr->CCR4 = 0;
            _base_addr->CCER |= TIM_CCER_CC4E;
            break;

        default:
            return false;
    }

    // TIM1 needs MOE for outputs
    if (_base_addr == TIM1)
    {
        _base_addr->BDTR |= TIM_BDTR_MOE;
    }

    // Alignment mode
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.mode),
           TIM_CR1_CMS_Pos, TIM_CR1_CMS_BitWidth);

    // Direction
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.dir),
           TIM_CR1_DIR_Pos, TIM_CR1_DIR_BitWidth);

    // Fixed ARR
    _base_addr->ARR = ARR_VAL;

    // Buffer ARR updates
    _base_addr->CR1 |= TIM_CR1_ARPE;

    // Reset counter + force update to latch preloads
    _base_addr->CNT = 0;
    _base_addr->EGR |= TIM_EGR_UG;

    // Enable counter
    _base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}

bool HwPwm::setFrequency(uint32_t frequency)
{
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if (frequency < 1)
    {
        return false;
    }

    if ((_settings.mode == PwmMode::EDGE_ALIGNED) &&
        (frequency > MAX_FREQ_EDGE_ALIGNED))
    {
        return false;
    }

    if ((_settings.mode != PwmMode::EDGE_ALIGNED) &&
        (frequency > MAX_FREQ_CENTER_ALIGNED))
    {
        return false;
    }

    // denom can exceed 2^32 when frequency is large
    // PSC+1 = round(pclk / (frequency * (ARR+1))) for edge-aligned
    const uint64_t arrp1 = static_cast<uint64_t>(ARR_VAL) + 1ULL;
    const uint64_t denom = static_cast<uint64_t>(frequency) * arrp1;

    // Rounded division: psc_val = round(pclk / denom)
    uint64_t psc_val =
        (static_cast<uint64_t>(pclk_freq) + (denom / 2ULL)) / denom;

    // Center-aligned: same PSC/ARR → need half PSC+1
    if (_settings.mode != PwmMode::EDGE_ALIGNED)
    {
        // rounded divide-by-2
        psc_val = (psc_val + 1ULL) / 2ULL;
    }

    // Valid PSC register is 0..65535 => psc_val (PSC+1) is 1..65536
    if (psc_val < 1ULL || psc_val > 65536ULL)
    {
        return false;
    }

    _base_addr->PSC = static_cast<uint32_t>(psc_val - 1ULL);
    _currentFrequency = frequency;

    // force update so PSC takes effect immediately (safe/typical)
    _base_addr->EGR |= TIM_EGR_UG;

    return true;
}

bool HwPwm::setDutyCycle(uint8_t dutyCycle)
{
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if (dutyCycle > 100)
    {
        return false;
    }

    // CCR (Capture/Compare Register value) = round(duty% * (ARR+1) / 100)
    const uint32_t arrp1 = static_cast<uint32_t>(ARR_VAL) + 1U;
    uint32_t ccr_val = (static_cast<uint32_t>(dutyCycle) * arrp1 + 50U) / 100U;

    switch (_channel)
    {
        case 1:
            _base_addr->CCR1 = ccr_val;
            break;
        case 2:
            _base_addr->CCR2 = ccr_val;
            break;
        case 3:
            _base_addr->CCR3 = ccr_val;
            break;
        case 4:
            _base_addr->CCR4 = ccr_val;
            break;
        default:
            return false;
    }

    _currentDutyCycle = dutyCycle;

    return true;
}

}  // namespace Stmf4
}  // namespace MM
