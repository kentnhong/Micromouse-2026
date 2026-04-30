/**
 * @file oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Interface
 */
#pragma once

#include <chrono>
#include <cstdint>

enum class TimerChannel : uint8_t
{
    CHANNEL_1 = 0,
    CHANNEL_2,
    CHANNEL_3,
    CHANNEL_4
    // Add more if supported on other boards
};

namespace MM
{
class Timebase
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
     * @brief Start timer counter
     * 
     */
    virtual void start_counter() = 0;

    /**
     * @brief Stop timer counter
     * 
     */
    virtual void stop_counter() = 0;

    /**
     * @brief Delete driver object
     * 
     */
    virtual ~Timebase() = default;

private:
    virtual bool set_period_us(std::chrono::microseconds period) = 0;
};
};  // namespace MM