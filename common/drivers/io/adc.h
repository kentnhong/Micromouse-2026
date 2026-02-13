/**
 * @file adc.h
 * @author Kent Hong
 * @brief Adc driver interface.
<<<<<<< HEAD
 */
#pragma once

#include <cstdint>

=======
 * 
 */
#pragma once

<<<<<<< HEAD
>>>>>>> 0d93109 (first commit)
=======
#include <cstdint>

>>>>>>> c4336e0 (add dma)
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
<<<<<<< HEAD
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
=======
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
>>>>>>> 0d93109 (first commit)
    ~Adc() = default;
};
};  // namespace MM