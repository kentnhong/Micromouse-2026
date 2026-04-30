/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver for STM32F4
 */
#pragma once

#include <chrono>
#include <cstdint>
#include "stm32f411xe.h"
#include "timebase.h"

#define PCLK_MAX 50000000u

struct StTimebaseParams
{
    TIM_TypeDef* base_addr;
    // Can add more params later
};

namespace MM
{
namespace Stmf4
{
class HwTimebase : public Timebase
{
public:
    explicit HwTimebase(StTimebaseParams params_);

    /**
     * @brief Initialize Timer Peripheral for Timebase mode
     * 
     * @param init_timer_freq The timer frequency you want to set on init.
     * @param period The timer period you want to set on init.
     * @param uie_en True for Update Event (Overflow/Underflow) Interrupt Enable, false for Update Event interrupt disable. Default argument is interrupt disabled.
     * @return true init success, false otherwise
     */
    template <typename Rep, typename Period>
    bool init(uint32_t pclk, uint32_t init_timer_freq,
              std::chrono::duration<Rep, Period> period, bool uie_en = false)
    {
        // Max PCLK Frequency for APB1 is 50 MHz
        if (pclk > PCLK_MAX)
        {
            return false;
        }

        // Select CK_INT as clock source (reset SMS in TIMx_SMCR)
        base_addr->SMCR &= ~TIM_SMCR_SMS;

        // Enable Edge-aligned mode from clearing CMS in TIMx_CR1
        base_addr->CR1 &= ~TIM_CR1_CMS;

        // Set as upcounter by clearing DIR in TIMx_CR1
        base_addr->CR1 &= ~TIM_CR1_DIR;

        // Enable Auto-reload preload enable bit (ARPE) in TIMx_CR1 register
        base_addr->CR1 |= TIM_CR1_ARPE;

        // Set URS in TIMx_CR1 to generate UEV only on counter overflow/underflow
        base_addr->CR1 |= TIM_CR1_URS;

        bool result = true;

        result = result && set_freq(init_timer_freq, pclk);

        // Convert period to microseconds
        auto period_us =
            std::chrono::duration_cast<std::chrono::microseconds>(period);
        result = result && set_period_us(period_us);

        // Check if frequency, period, and compare were set successfully before starting counter
        if (!result)
        {
            return false;
        }

        if (uie_en)
        {
            base_addr->DIER |= TIM_DIER_UIE;
        }

        // Generate a software update event to load shadow registers (ARR, PSC)
        base_addr->EGR |= TIM_EGR_UG;

        return true;
    }

    /**
     * @brief Set the Timer Frequency
     * 
     * @param new_timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    bool set_freq(uint32_t new_timer_freq, uint32_t pclk);

    /**
     * @brief Start timer counter
     * 
     */
    void start();

    /**
     * @brief Stop timer counter
     * 
     */
    void stop();

    /**
     * @brief Set the Timer Period
     * 
     * @param period_us The timer period in microseconds
     * @return true Timer Period set successfully, false otherwise
     */
    bool set_period_us(std::chrono::microseconds period_us);

private:
    uint32_t timer_freq;
    uint32_t prescaler;
    uint32_t period_ticks;
    uint32_t compare_ticks;
    TIM_TypeDef* base_addr;
};
};  // namespace Stmf4
};  // namespace MM