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

    virtual ~Encoder() = default;
};
}  // namespace MM
