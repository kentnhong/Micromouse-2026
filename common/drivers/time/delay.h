#pragma once
#include <cstdint>
#include "timebase.h"

namespace MM::Utils
{
/**
    * @brief Delays execution for a specified number of milliseconds.
    * @note  Since timebase is a 32 bit timer, the maximum ms delay allowed currently is 4,294,967 ms
    * @param ms The number of milliseconds to delay.
    * @return true Delay success, false Timebase wasn't binded correctly
    */
bool delay_ms(uint32_t ms);

/**
    * @brief Delays execution for a specified number of microseconds.
    * @note  Since timebase is a 32 bit timer, the maximum us delay allowed currently is 4,294,967,295 us
    * @param us The number of microseconds to delay.
    * @return true Delay success, false Timebase wasn't binded correctly
    */
bool delay_us(uint32_t us);

/**
 * @brief Bind a timebase object to delay functions
 * @note Timebase timer frequency must be 1MHz to get 1us per tick
 * @note Timebase resolution must be 32 bits (for now)
 * 
 * @param timebase The timebase object to bind
 * @return true Timebase binded successfully, false otherwise 
 */
bool bind_timebase(const Timebase& timebase);

/**
    * @brief  Returns the current millisecond tick count (since boot).
    * @return The current millisecond tick count.
    */
uint32_t get_ms_ticks();
}  // namespace MM::Utils