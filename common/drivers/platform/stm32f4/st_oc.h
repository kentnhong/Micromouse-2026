/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver for STM32F4
 */
#pragma once

#include <chrono>
#include <cstdint>
#include "oc.h"
#include "stm32f411xe.h"

struct StOcParams
{
    TIM_TypeDef* base_addr;
    uint32_t pclk;  // Max PCLK Frequency for APB1 is 50 MHz
};

namespace MM
{
namespace Stmf4
{
class HwOc : public Oc
{
public:
    explicit HwOc(StOcParams params_);

    /**
     * @brief Initialize Timer Peripheral for Output Compare mode
     * 
     * @return true init success, false otherwise
     */
    template <typename Rep, typename Period>
    bool init(uint32_t init_timer_freq,
              std::chrono::duration<Rep, Period> period,
              std::chrono::duration<Rep, Period> compare)
    {
        // Select CK_INT as clock source (reset SMS in TIMx_SMCR)
        base_addr->SMCR &= ~TIM_SMCR_SMS;

        // Enable Edge-aligned mode from clearing CMS in TIMx_CR1
        base_addr->CR1 &= ~TIM_CR1_CMS;

        // Set as upcounter by clearing DIR in TIMx_CR1
        base_addr->CR1 &= ~TIM_CR1_DIR;

        // Enable Auto-reload preload enable bit (ARPE) in TIMx_CR1 register
        base_addr->CR1 |= TIM_CR1_ARPE;

        // Do not set UIF flag for interrupt and DMA requests (enable URS in TIMx_CR1)
        base_addr->CR1 |= TIM_CR1_URS;

        bool result = true;

        result = result && set_freq(init_timer_freq);
        result = result && set_period(period);
        result = result && set_compare(compare);

        // Check if frequency, period, and compare were set successfully before starting counter
        if (!result)
        {
            return false;
        }

        start_counter();

        return true;
    }

    /**
     * @brief Set the Timer Frequency
     * 
     * @param timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    bool set_freq(uint32_t new_timer_freq);

private:
    // Private Methods

    /**
     * @brief Start timer counter
     * 
     */
    void start_counter();

    /**
     * @brief Stop timer counter
     * 
     */
    void stop_counter();

    /**
     * @brief Set the Timer Period
     * 
     * @param period_us The timer period in microseconds
     * @return true Timer Period set successfully, false otherwise
     */
    bool set_period_us(std::chrono::microseconds period_us);

    /**
     * @brief Set the Timer Compare within the Timer Period 
     * 
     * @param compare_us The timer compare in microseconds
     * @return true Timer Compare set successfully, false otherwise
     */
    bool set_compare_us(std::chrono::microseconds compare_us);

    uint32_t timer_freq;
    uint32_t pclk;
    uint32_t prescaler;
    uint32_t period_ticks;
    uint32_t compare_ticks;
    TIM_TypeDef* base_addr;
};
};  // namespace Stmf4
};  // namespace MM