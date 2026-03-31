/**
 * @file enc_math.h
 * @brief Common math types for encoder calculations
 * @author Bex Saw
 * @date 3/28/2026
 */

#pragma once

#include <cstdint>

<<<<<<< HEAD
#include <cstdint>

=======
>>>>>>> 3da8257 (PID structure + ideas)
namespace MM
{

struct EncoderInput
{
    int32_t left_ticks = 0;
    int32_t right_ticks = 0;
<<<<<<< HEAD
<<<<<<< HEAD
=======
    float left_velocity = 0.0f;   // in ticks/s but converted to m/s
    float right_velocity = 0.0f;  // same as above
>>>>>>> 3da8257 (PID structure + ideas)
=======
>>>>>>> 31fdaba (output to duty cycle)
};

}  // namespace MM
