/**
 * @file adc.h
 * @author Kent Hong
 * @brief Adc driver interface.
 */
#pragma once

#include <cstddef>
#include <cstdint>

namespace MM
{
/**
 * @class Adc
 * @brief Adc driver instance
 * 
 */
class Adc
{
public:
    /**
    * @brief Single or continuous conversion of an ADC channel
    * 
    * @param single True for single. False for continuous.
    * @param samples How many ADC samples for single shot conversions (obsolete if single is set to false)
    * @return true Conversion successful, false otherwise
    */
    virtual bool convert(bool single, size_t samples) = 0;

    /**
    * @brief Read converted analog values from ADC buffer
    *
    * @param val Variable to store adc conversion
    * @return true Read successful, false otherwise
    */
    virtual bool read(uint32_t& val) = 0;

    /**
     * @brief Set ADC channel at a specific sequence position
     *
     * @param rank Order of ADC channel to be converted for a sequence of conversions
     * @param ch ADC channel
     */
    virtual bool set_channel(uint8_t rank, uint8_t ch) = 0;

    /**
    * @brief Destroy the Adc object
    * 
    */
    virtual ~Adc() = default;
};
};  // namespace MM