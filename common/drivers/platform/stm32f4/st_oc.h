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
     * @param init_timer_freq Desired timer freq on first boot
     * @return true init success, false otherwise
     */
    bool init(uint32_t init_timer_freq);

    /**
     * @brief Set the Timer Period (Update_event = TIM_CLK/((PSC + 1)*(ARR + 1)*(RCR + 1)))
     * 
     * @param period_us The timer period in microseconds
     * @return true Timer Period set successfully, false otherwise
     */
    bool set_period(std::chrono::microseconds period_us);

    /**
     * @brief Set the Timer Compare within the Timer Period 
     * 
     * @param compare_us The timer compare in microseconds
     * @return true Timer Compare set successfully, false otherwise
     */
    bool set_compare(std::chrono::microseconds compare_us);

    /**
     * @brief Set the Timer Frequency
     * 
     * @param timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    bool set_freq(uint32_t new_timer_freq);

private:
    // Private Methods
    void start_counter();
    void stop_counter();

    uint32_t timer_freq;
    uint32_t pclk;
    uint32_t prescaler;
    // If changing period in different time unit, have to change all period member variables to match
    std::chrono::microseconds period_us;
    TIM_TypeDef* base_addr;
};
};  // namespace Stmf4
};  // namespace MM