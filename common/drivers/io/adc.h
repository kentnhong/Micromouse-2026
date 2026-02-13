/**
 * @file adc.h
 * @author Kent Hong
 * @brief Adc driver interface.
 * 
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
        * @brief Continuous conversion of analog to digital values
        * 
        * @return true Conversion successful, false otherwise
        */
    virtual bool convert(uint8_t channel) = 0;

    /**
        * @brief Read converted analog values from ADC buffer
        * 
        * @return true Read successful, false otherwise
        */
    virtual bool read() = 0;

    /**
         * @brief Destroy the Adc object
         * 
         */
    ~Adc() = default;
};
};  // namespace MM