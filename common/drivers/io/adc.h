/**
 * @file adc.h
 * @author Kent Hong
 * @brief Adc driver interface.
 */
#pragma once

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
    * @return true Conversion successful, false otherwise
    */
    virtual bool convert(bool single) = 0;

    /**
    * @brief Read converted analog values from ADC buffer
    *
    * @param val Variable to store adc conversion
    * @return true Read successful, false otherwise
    */
    virtual bool read(uint16_t& val) = 0;

    /**
    * @brief Destroy the Adc object
    * 
    */
    ~Adc() = default;
};
};  // namespace MM