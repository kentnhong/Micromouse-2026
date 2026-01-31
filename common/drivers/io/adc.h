/**
 * @file adc.h
 * @author Kent Hong
 * @brief Generic interface for Analog-to-Digital Converter (ADC) drivers
 * 
 */

#pragma once

#include <cstdint>

namespace MM
{
/**
 * @class Adc
 * @brief Adc driver instance.
 * 
 */
class Adc
{
public:
    /**
     * @brief Continuously converts analog values to digital values
     * 
     * @return True Successful conversion. False otherwise.
     */
    virtual bool convert() = 0;

    /**
     * @brief Reads digital value from ADC buffer
     * 
     * @return true Successful read. False otherwise.
     */
    virtual bool read() = 0;

    /**
     * @brief Destroy the pure virtual instance pointing or referring to the Adc object
     * 
     */
    ~Adc() = default;
};
};  // namespace MM
