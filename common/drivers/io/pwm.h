/**
 * @file pwm.h
 * @brief PWM header interface
 * @date 2/6/2026
 * @author Bex Saw
*/

#pragma once 
#include <cstdint>
namespace MM 
{
class Pwm
{

    /**
    * @brief Sets the frequency of the PWM timer
    * @param freq Desired frequency in Hz
    * @return true if successful, false otherwise
    */
    virtual bool setFrequency(uint32_t frequency) = 0;

    /**
    * @brief Sets the duty cycle of the PWM signal
    * @param dutyCycle Desired duty cycle as a percentage
    * @return true if successful, false otherwise
    */
    virtual bool setDutyCycle(uint8_t dutyCycle) = 0;

    ~Pwm() = default;
};

} // namespace MM