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
    enum class Direction : uint8_t
    {
        COAST = 0,
        FORWARD = 1,
        REVERSE = 2,
        BRAKE = 3
    };

    /**
     * @brief Constructor for DRV8231 motor driver using separate direction pins
     * and one speed PWM pin.
     * @param in1 GPIO pin for direction 1
     * @param in2 GPIO pin for direction 2
     * @param speed PWM driver object for speed control
     */
    Drv8231(Gpio& in1, Gpio& in2, Pwm& speed);

    /**
     * @brief Constructor for DRV8231 motor driver using IN/IN PWM control.
     * @param pwm1 PWM output for IN1
     * @param pwm2 PWM output for IN2
     */
    Drv8231(Pwm& pwm1, Pwm& pwm2);

    /**
     * @brief Drive the motor with a specific direction and duty cycle
     * @param dir Direction of motor rotation
     * @param duty_cycle Motor speed as a percentage (0-100)
     */
    void drive(Direction dir, uint8_t duty_cycle);

    /**
     * @brief Get the current motor state
     * @return Current motor state
     */
    int get_state() const;

    bool init();

private:
    int state;
    Gpio* in1;
    Gpio* in2;
    Pwm* speed;
    Pwm* pwm1;
    Pwm* pwm2;
};

}  // namespace MM