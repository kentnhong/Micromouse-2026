/**
 * @file oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Interface
 */
#pragma once

#include <chrono>
#include <cstdint>

namespace MM
{
class Oc
{
public:
    /**
     * @brief Set the Timer Frequency
     * 
     * @param timer_freq The desired new timer frequency
     * @return true Timer Frequency set successfully, false otherwise
     */
    virtual bool set_freq(uint32_t new_timer_freq) = 0;

    /**
     * @brief Set the Timer Period
     * 
     * @param period The timer period in any unit of time
     * @return true Timer Period set successfully, false otherwise
     */
    template <typename Rep, typename Period>
    bool set_period(std::chrono::duration<Rep, Period> period)
    {
        auto period_us =
            std::chrono::duration_cast<std::chrono::microseconds>(period);
        return set_period_us(period_us);
    }

    /**
     * @brief Set the Timer Compare within the Timer Period 
     * 
     * @param compare The timer compare in any unit of time
     * @return true Timer Compare set successfully, false otherwise
     */
    template <typename Rep, typename Period>
    bool set_compare(std::chrono::duration<Rep, Period> compare)
    {
        auto compare_us =
            std::chrono::duration_cast<std::chrono::microseconds>(compare);
        return set_compare_us(compare_us);
    }

    /**
     * @brief Delete driver object
     * 
     */
    virtual ~Oc() = default;

private:
    virtual bool set_period_us(std::chrono::microseconds period) = 0;
    virtual bool set_compare_us(std::chrono::microseconds compare) = 0;
};
};  // namespace MM