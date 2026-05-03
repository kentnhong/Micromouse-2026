/**
* @file enc_sample.h
* @brief Encoder sampling utilities for PID controller
* @author Bex Saw
* @date 4/15/2026
*/

#pragma once

#include <cstdint>
#include "enc_math.h"
#include "encoder.h"

namespace MM
{
namespace Sample
{

struct EncoderTiming
{
    uint32_t sample_cycles;
    float sample_time_sec;
};

/**
* @brief Initializes the encoder timing based on the desired sample time in microseconds
* @param timing_encoder The encoder used for timing
* @param sample_time_us The desired sample time in microseconds
* @return An EncoderTiming struct containing the calculated sample cycles and sample time in seconds
*/
EncoderTiming init_encoder_timing(Encoder& timing_encoder,
                                  uint32_t sample_time_us);

/**
* @brief Samples a single encoder for a specified duration based on the provided timing
* @param encoder The encoder to sample
* @param timing The EncoderTiming struct containing the sample duration information
* @return The number of ticks during the sampling period
*/
int32_t sample_encoder(Encoder& encoder, const EncoderTiming& timing);

/**
* @brief Samples two encoders for a specified duration based on the provided timing
* @param phase_a Phase A encoder to sample
* @param phase_b Phase B encoder to sample
* @param timing The EncoderTiming struct containing the sample duration information
* @return An EncoderInput struct containing the number of ticks for both encoders during the sampling period
*/
MM::EncoderInput sample_encoders(Encoder& phase_a, Encoder& phase_b,
                                 const EncoderTiming& timing);

}  // namespace Sample
}  // namespace MM
