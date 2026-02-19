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
public:
    /**
    * @brief Sets the frequency of the PWM timer
    * @param frequency Desired frequency in Hz
    * @return true if successful, false otherwise
    */
    virtual bool set_frequency(uint32_t frequency) = 0;

    /**
    * @brief Sets the duty cycle of the PWM signal
    * @param duty_cycle Desired duty cycle as a percentage (0-100)
    * @return true if successful, false otherwise
    */
    virtual bool set_duty_cycle(uint8_t duty_cycle) = 0;

    ~Pwm() = default;
};

}  // namespace MM