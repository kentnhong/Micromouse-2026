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
    bool init();

    /**
     * @brief Set the Timer Frequency
     * 
     * @param timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    bool set_freq(uint32_t new_timer_freq);

private:
    // Private Methods
    // TODO: Make it so that start_counter and stop_counter arent exposed to public API in function implementation
    // Integrate set_freq, set_period, and set_compare inside of init and make init a template?
    bool start_counter();
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

    // Flags for seeing if freq, period, and compare were set before first start_counter()
    bool has_freq = false;
    bool has_period = false;
    bool has_compare = false;
};
};  // namespace Stmf4
};  // namespace MM