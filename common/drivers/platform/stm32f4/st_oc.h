/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver for STM32F4
 */
#pragma once

#include <chrono>
#include <cstdint>
#include "oc.h"

struct StOcParams
{
    uint32_t pclk;
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
     * @brief Set the Timer Period
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
     * @param timer_freq The desired timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    bool set_freq(uint32_t timer_freq);

private:
    uint32_t timer_freq;
    uint32_t pclk;
    // If changing period in different time unit, have to change all period member variables to match
    std::chrono::microseconds period_us;
};
};  // namespace Stmf4
};  // namespace MM