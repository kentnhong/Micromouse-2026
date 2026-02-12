#include "st_pwm.h"
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4
{
static constexpr uint32_t pclk_freq = 84000000;  // 84 MHz for APB1 timers
static constexpr uint8_t TIM_CCMRx_OCxM_BitWidth = 3;
static constexpr uint8_t TIM_CR1_CMS_BitWidth = 2;
static constexpr uint8_t TIM_CR1_DIR_BitWidth = 1;
static constexpr uint8_t TIM_CCRx_BitWidth = 16;
static constexpr uint8_t ARR_VAL = 99;
static constexpr uint32_t MAX_FREQ_EDGE_ALIGNED = pclk_freq / (ARR_VAL + 1);
static constexpr uint32_t MAX_FREQ_CENTER_ALIGNED = pclk_freq / (2 * (ARR_VAL + 1));

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

    /**
     * Make sure channel number is valid according to given timer
     * @note Timer 9, 10, 11 have 2 channels 
     *       Timers 1, 2, 3, 4, 5 have 4 channels
    */
    if ((_base_addr == TIM9 || _base_addr == TIM10 || _base_addr == TIM11) && _channel > 2)
    {
        return false;
    }
    else if (_base_addr == TIM1 || _base_addr == TIM2 || _base_addr == TIM3 || _base_addr == TIM4 || _base_addr == TIM5)
    {
        if (_channel > 4)
        {
            return false;
        }
    }
    else
    {
        return false;
    }

    /**
     * @brief Make sure that the  PwmMode & PwmDir are valid settings acorrding to channel
     * @note Timers 1, 2, 3, 4, 5 support both edge-aligned and center-aligned modes
     *       Timers 9, 10, 11 only support edge-aligned mode
     */
    if ((_base_addr == TIM9 || _base_addr == TIM10 || _base_addr == TIM11) &&
        (_settings.mode != PwmMode::EDGE_ALIGNED || _settings.dir != PwmDir::UPCOUNTING))
    {
        // Timers 9, 10, 11 only support edge-aligned mode
        return false;
    }
  
    // Configure timer according to channel and settings in params
    switch (_channel)
    {
        case 1:
            // Configure channel as an output
            _base_addr->CCMR1 &=
                ~TIM_CCMR1_CC1S_Msk;  // Clear OC1M and OC1PE bits

            // Configure output mode
            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.outputMode), 
                    TIM_CCMR1_OC1M_Pos, TIM_CCMRx_OCxM_BitWidth);

            // Configure default duty cycle to 0%
            _base_addr->CCR1 = _currentDutyCycle;

            // set preload bit to buffer updates to capture/compare register until the next update event
            _base_addr->CCMR1 |= TIM_CCMR1_OC1PE;

            // Enable capture/compare channel
            _base_addr->CCER |= TIM_CCER_CC1E;
            break;
        case 2:
            // Configure channel as an output
            _base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;

            // Configure output mode
            SetReg(&_base_addr->CCMR1,
                   static_cast<uint32_t>(_settings.outputMode), 
                    TIM_CCMR1_OC2M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCR2 = _currentDutyCycle;
            _base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            _base_addr->CCER |= TIM_CCER_CC2E;
            break;
        case 3:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;
            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.outputMode), 
                   TIM_CCMR2_OC3M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCR3 = _currentDutyCycle;
            _base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            _base_addr->CCER |= TIM_CCER_CC3E;
            break;
        case 4:
            _base_addr->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;
            SetReg(&_base_addr->CCMR2,
                   static_cast<uint32_t>(_settings.outputMode), 
                   TIM_CCMR2_OC4M_Pos, TIM_CCMRx_OCxM_BitWidth);
            _base_addr->CCR4 = _currentDutyCycle;
            _base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            _base_addr->CCER |= TIM_CCER_CC4E;
            break;
        default:
            return false;
    }
    /**
     * Set MOE bit for Timers 1
     * @note This is required for any output compare mode to work for Timer 1
     */
    if (_base_addr == TIM1)
    {
        _base_addr->BDTR |= TIM_BDTR_MOE;
    }

    // Config PWM mode and direction
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.mode),
            TIM_CR1_CMS_BitWidth, TIM_CR1_CMS_Msk);
    SetReg(&_base_addr->CR1, static_cast<uint32_t>(_settings.dir),
            TIM_CR1_DIR_Pos, TIM_CR1_DIR_BitWidth);

    // Configure fixed ARR value
    _base_addr->ARR = ARR_VAL;

    // Initialize counter to 0 and update registers
    _base_addr->CNT = 0;
    _base_addr->EGR |= TIM_EGR_UG;

    // Enable auto-reload preload to buffer updates to ARR until the next update event
    _base_addr->CR1 |= TIM_CR1_ARPE;

    // Enable timer counter
    _base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}

bool HwPwm::setFrequency(uint32_t frequency)
{
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }
    if ((frequency < 1) || (frequency > MM::Stmf4::pclk_freq))
    {
        return false;
    }
    if ((_settings.mode == PwmMode::EDGE_ALIGNED) &&
        (frequency > MM::Stmf4::MAX_FREQ_EDGE_ALIGNED))
    {
        return false;
    }
    if ((_settings.mode != PwmMode::EDGE_ALIGNED) &&
        (frequency > MM::Stmf4::MAX_FREQ_CENTER_ALIGNED))
    {
        return false;
    }

    /**
    * @brief Solve for prescaler (PSC) value using the formula:
    *        freq = P_CLK / ((PSC + 1)(ARR + 1))
    *       where P_CLK is the APB1 clock frequency (84 MHz for STM32F4)
    */

    uint32_t psc_val =
        MM::Stmf4::pclk_freq / (frequency * (MM::Stmf4::ARR_VAL + 1));
    if (_settings.mode != PwmMode::EDGE_ALIGNED)
    {
        psc_val /= 2;
    }

    // Make sure calculated PSC value is valid
    if (psc_val < 1 || psc_val > 65536)
    {
        return false;
    }

    _base_addr->PSC = psc_val - 1;
    _currentFrequency = frequency;
    return true;
}

bool HwPwm::setDutyCycle(uint8_t dutyCycle)
{
    if (!(_base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }
    if ((dutyCycle < 0) || (dutyCycle > 100))
    {
        return false;
    }

    // CCR value 
    uint32_t ccr_val =
        static_cast<uint32_t>(dutyCycle * (MM::Stmf4::ARR_VAL + 1)) / 100.0f;

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
