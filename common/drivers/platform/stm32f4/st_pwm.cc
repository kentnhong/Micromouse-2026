#include "st_pwm.h"
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4 
{

static constexpr uint32_t pclk_freq = 84000000; // 84 MHz for APB1 timers

// Bit lengths
static constexpr uint8_t TIM_CCMRx_OCxM_BitWidth = 3;
static constexpr uint8_t TIM_CR1_CMS_BitWidth = 2;
static constexpr uint8_t TIM_CR1_DIR_BitWidth = 1;
static constexpr uint8_t TIM_CCRx_BitWidth = 16;

/**
 * Fixed auto-reload value (ARR)
 * @note ARR + 1 = 100 timer ticks per PWM period, resulting in
 *       a duty cycle resolution of 1% per step
 * @warning If this changes, the equation associated with set_freq
 *          in the header file must change
 */
static constexpr uint8_t ARR_VAL = 99;

/**
 * Maxmimum PWM frequencies according to ARR value
 */
static constexpr uint32_t MAX_FREQ_EDGE_ALIGNED = pclk_freq / (ARR_VAL + 1);
static constexpr uint32_t MAX_FREQ_CENTER_ALIGNED = pclk_freq / (2 * (ARR_VAL + 1));

HwPwm::HwPwm(const StPwmParams& params_)
    : base_addr{params_.base_addr},
      channel{params_.channel},
      settings{params_.settings},
      currentFrequency{pclk_freq},
      currentDutyCycle{0}
{
}

bool HwPwm::init()
{

    if (base_addr == nullptr) 
    {
        return false; 
    }

    // Timer 6 & 7 are basic timers and do not support PWM mode
    if(base_addr == TIM6 || base_addr == TIM7) 
    {
        return false;
    } 

     /**
     * Make sure channel number is valid according to given timer
     * @note Timer 9, 10, 11 have 2 channels 
     *       Timers 1, 2, 3, 4, 5 have 4 channels
     */
     
    if( channel < 1 || channel > 4) 
    {
        return false;
    }
    else if ((base_addr == TIM9 || base_addr == TIM10 || base_addr == TIM11) && channel > 2) 
    {
        return false;
    }
    else if (base_addr == TIM1 && channel > 4) 
    {
        return false;
    }

    /**
     * @brief Make sure that the  PwmMode & PwmDir are valid settings acorrding to channel
     * @note Timers 1, 2, 3, 4, 5 support both edge-aligned and center-aligned modes
     *       Timers 9, 10, 11 only support edge-aligned mode
     */
     if((base_addr == TIM9 || base_addr == TIM10 || base_addr == TIM11) && settings.mode != PwmMode::EDGE_ALIGNED) 
     {
        return false;
     }
     else if ((base_addr == TIM1) && settings.mode != PwmMode::EDGE_ALIGNED && setting.mode != PwmMode::CENTER_ALIGNED_DOWN) 
     {
        return false;
     }

     // Configure timer according to channel and settings in params
    switch(channel)
    {
        case 1:
            // Configure channel as an output
            base_addr->CCMR1 &= ~TIM_CCMR1_CC1S_Msk; // Clear OC1M and OC1PE bits

            // Configure output mode 
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode) << TIM_CCMR1_OC1M_Pos,
                   TIM_CCMRx_OCxM_BitWidth);
            
            // Configure default duty cycle to 0%
            base_addr->CCR1 = currentDutyCycle;

            // set preload bit to buffer updates to capture/compare register until the next update event
            base_addr->CCMR1 |= TIM_CCMR1_OC1PE;

            // Enable capture/compare channel
            base_addr->CCER |= TIM_CCER_CC1E;
            break;
        case 2:
            // Configure channel as an output
            base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;
            
            // Configure output mode
            SetReg(&base_addr->CCMR1,
                   static_cast<uint32_t>(settings.output_mode) << TIM_CCMR1_OC2M_Pos, 
                   TIM_CCMRx_OCxM_BitWidth);

            base_addr->CCR2 = currentDutyCycle;
            base_addr->CCMR1 |= TIM_CCMR1_OC2PE;
            // Enable capture/compare channel
            base_addr->CCER |= TIM_CCER_CC2E;
            break;
        case 3:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC3S_Msk;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode) << TIM_CCMR2_OC3M_Pos,
                   TIM_CCMRx_OCxM_BitWidth);
            base_addr->CCR3 = currentDutyCycle;
            base_addr->CCMR2 |= TIM_CCMR2_OC3PE;
            // Enable capture/compare channel
            base_addr->CCER |= TIM_CCER_CC3E;
            break;
        case 4:
            base_addr->CCMR2 &= ~TIM_CCMR2_CC4S_Msk;
            SetReg(&base_addr->CCMR2,
                   static_cast<uint32_t>(settings.output_mode) << TIM_CCMR2_OC4M_Pos,
                   TIM_CCMRx_OCxM_BitWidth);
            base_addr->CCR4 = currentDutyCycle;
            base_addr->CCMR2 |= TIM_CCMR2_OC4PE;
            // Enable capture/compare channel
            base_addr->CCER |= TIM_CCER_CC4E;
            break;
        default:
            return false;
    }
    /**
     * Set MOE bit for Timers 1
     * @note This is required for any output compare mode to work for Timer 1
     */
    if (base_addr == TIM1)
    {
        base_addr->BDTR |= TIM_BDTR_MOE;
    }

    // Config PWM mode and direction
    SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.mode) << 
            TIM_CR1_CMS_Pos, TIM_CR1_CMS_BitWidth);
    SetReg(&base_addr->CR1, static_cast<uint32_t>(settings.direction) <<
            TIM_CR1_DIR_Pos, TIM_CR1_DIR_BitWidth);

    // Configure fixed ARR value 
    base_addr->ARR = ARR_VAL;

    // Initialize counter to 0 and update registers
    base_addr->CNT = 0;
    base_addr->EGR |= TIM_EGR_UG;

    // Enable auto-reload preload to buffer updates to ARR until the next update event
    base_addr->CR1 |= TIM_CR1_ARPE;

    // Enable timer counter
    base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}
     

}

bool HwPwm::setFrequency(uint32_t frequency)
{
      // Make sure counter was initialized
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if ((frequency < 1) || (frequency > pclk_freq))
    {
        return false;
    }

    // Check if given frequency is higher than the max possible according to the ARR value
    if ((settings.mode == PwmMode::EDGE_ALIGNED) &&
        (frequency > MAX_FREQ_EDGE_ALIGNED))
    {
        return false;
    }

    if ((settings.mode != PwmMode::EDGE_ALIGNED) &&
        (frequency > MAX_FREQ_CENTER_ALIGNED))
    {
        return false;
    }

    /**
     * Solve for prescalar (PSC) value
     * @note If in edge-aligned mode: freq = P_CLK / ((PSC + 1) (ARR + 1))
     * @note If in center-aligned mode: freq = P_CLK / (2(PSC + 1)(ARR + 1))
     */
    uint32_t psc_val = pclk_freq / (frequency * (ARR_VAL + 1));
    if (settings.mode != PwmMode::EDGE_ALIGNED)
    {
        psc_val /= 2;
    }

    // Make sure calculated value is valid
    if (psc_val < 1 || psc_val > 65536)
    {
        return false;
    }

    base_addr->PSC = psc_val - 1;
    currentFrequency = frequency;

    return true;
}

bool HwPwm::setDutyCycle(uint8_t dutyCycle)
{

    // Make sure counter was initialized
    if (!(base_addr->CR1 & TIM_CR1_CEN_Msk))
    {
        return false;
    }

    if ((dutyCycle < 0) || (dutyCycle > 100))
    {
        return false;
    }

    // Calculate the CCR value according to the given duty cycle and ARR value
    uint32_t ccr_val = static_cast<uint32_t>(dutyCycle * (ARR_VAL + 1)) / 100.0f;

    switch(channel)
    {
        case 1:
            base_addr->CCR1 = ccr_val;
            break;
        case 2:
            base_addr->CCR2 = ccr_val;
            break;
        case 3:
            base_addr->CCR3 = ccr_val;
            break;
        case 4:
            base_addr->CCR4 = ccr_val;
            break;
        default:
            return false;
    }

    currentDutyCycle = dutyCycle;

    return true;
}
}


