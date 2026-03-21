/**
* @file drv8231.h
* @author Bex Saw
* @brief DRV8231 motor driver interface
* @version 0.1
*/

#pragma once
#include <cstdint>
#include "gpio.h"
#include "pwm.h"

namespace MM
{

class Drv8231
{
public:
    /**
    * @brief Direction of motor rotation
    * FORWARD: Motor rotates in the forward direction
    * REVERSE: Motor rotates in the reverse direction
    */
    enum class Direction
    {
        COAST = 0,
        FORWARD,
        REVERSE,
        BRAKE
    };

    /**
     * @brief Constructor for DRV8231 motor driver
     * @param in1 Reference to IN1 pin GPIO object
     * @param in2 Reference to IN2 pin GPIO object
     * @param pwm_driver Reference to PWM driver object
     */
    explicit Drv8231(Gpio& in1, Gpio& in2, Pwm& speed_pwm);

    /**
     * @brief Set the motor direction (COAST, FORWARD, REVERSE, BRAKE)
     * @param dir Direction of motor rotation
     */
    void set_direction(Direction dir);

    /**
     * @brief Set the motor speed (PWM duty cycle)
     * @param speed Speed of motor rotation (0-255)
     */
    void set_speed(uint8_t speed);

    bool init();

private:
    Gpio& in1_pin;
    Gpio& in2_pin;
    Pwm& pwm;
};

}  // namespace MM