/**
* @file encoder.h
* @brief Encoder header interface
* @date 3/17/2026
* @author Bex Saw
*/

#pragma once
#include <cstdint>

namespace MM
{

/* Calculation Stats */
struct EncoderStats
{
    float rpm;
    float velocity_cm_per_sec;
    float distance_cm;
    float delta_time_sec;
    int32_t delta_ticks;
};

class Encoder
{
public:
    /**
    * @brief Gets the current encoder ticks.
    * @return Returns the current encoder ticks.
    */
    virtual int32_t get_ticks() = 0;

    /**
    * @brief Resets the encoder ticks to zero.
    * @return Returns true if success.
    */
    virtual bool reset_ticks() = 0;

    /**
    * @brief Initializes the DWT cycle counter for timing measurements
    * @note Derive down from the SysClock at some HSI/HSE to get accurate timing
    */
    virtual bool init_cycle_counter() = 0;

    /**
    * @brief Gets the current time in cycles using the DWT cycle counter
    * @return The current time in cycles
    */
    virtual uint32_t get_time_cycles() = 0;

    /**
    * @brief Returns the number of cycles per microsecond
    */
    virtual uint32_t cycles_per_us() = 0;

    /**
    * @brief Measures encoder statistics over a specified sample time in cycles.
    * @param sample_cycles - The number of cycles to sample for.
    * @param ticks_per_output_rev - Encoder ticks per output shaft revolution.
    * @param cm_per_tick - Linear wheel travel per encoder tick.s
    * @return An EncoderStats struct containing the measured statistics.
    * @note This function blocks for the duration of the sample time while it collects data.
    */
    virtual EncoderStats measure_encoder_stats(uint32_t sample_cycles,
                                               float ticks_per_output_rev,
                                               float cm_per_tick) = 0;

    ~Encoder() = default;
};
}  // namespace MM