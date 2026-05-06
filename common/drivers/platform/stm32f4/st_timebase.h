/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timebase Driver for the STM32F411 for Timers 1, 2, 3, 4, and 5
 */
#pragma once

#include <chrono>
#include <cstdint>
#include "stm32f411xe.h"
#include "timebase.h"

constexpr uint32_t PCLK_MAX{50'000'000u};

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
        // Check if timer base address is valid
        if (!(base_addr == TIM1 || base_addr == TIM2 || base_addr == TIM3 ||
              base_addr == TIM4 || base_addr == TIM5))
        {
            return false;
        }

        // Max PCLK Frequency for APB1 is 50 MHz
        if (pclk > PCLK_MAX)
        {
            return false;
        }

        if (base_addr == TIM1)
        {
            base_addr->RCR = 0;
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
    bool set_freq(uint32_t new_timer_freq, uint32_t pclk) override;

    /**
     * @brief Set the Timer Period
     * 
     * @param period_us The timer period in microseconds
     * @return true Timer Period set successfully, false otherwise
     */
    bool set_period_us(std::chrono::microseconds period_us) override;

    /**
     * @brief Start timer counter
     * 
     */
    void start() override;

    /**
     * @brief Stop timer counter
     * 
     */
    void stop() override;

    /**
     * @brief Get the current timer count value
     * 
     * @return uint32_t The current timer counter value
     */
    uint32_t get_count() const override;

    /**
     * @brief Get the timer frequency
     * 
     * @return uint32_t Timer frequency
     */
    uint32_t get_freq() const override;

    /**
     * @brief Get the maximum possible counter value
     * @note Used to differentiate between a 16-bit and 32-bit timer
     * 
     * @return uint32_t The maximum count
     */
    uint32_t get_max_count() const override;

private:
    uint32_t timer_freq;
    uint32_t prescaler;
    uint32_t period_ticks;
    TIM_TypeDef* base_addr;
};
};  // namespace Stmf4
};  // namespace MM