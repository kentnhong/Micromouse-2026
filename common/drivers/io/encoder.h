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

    ~Encoder() = default;
};
}  // namespace MM