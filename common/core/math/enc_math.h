/**
 * @file enc_math.h
 * @brief Common math types for encoder calculations
 * @author Bex Saw
 * @date 3/28/2026
 */

#pragma once

#include <cstdint>

namespace MM
{

struct EncoderInput
{
    int32_t left_ticks = 0;
    int32_t right_ticks = 0;
};

}  // namespace MM