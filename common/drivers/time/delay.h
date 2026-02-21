#pragma once
#include <cstdint>

namespace MM::Utils
{
/**
    * @brief Delays execution for a specified number of milliseconds.
    * @param ms The number of milliseconds to delay.
    */
void DelayMs(uint32_t ms);

/**
    * @brief Delays execution for a specified number of microseconds.
    * @param us The number of microseconds to delay.
    */
void DelayUs(uint32_t us);

/**
    * @brief  Returns the current millisecond tick count (since boot).
    * @return The current millisecond tick count.
    */
uint32_t GetMsTicks();
}  // namespace MM::Utils