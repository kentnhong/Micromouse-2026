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
    enum class Direction : uint8_t
    {
        COAST = 0,
        FORWARD = 1,
        REVERSE = 2,
        BRAKE = 3
    };

    /**
     * @brief Constructor for DRV8231 motor driver
     * @param config Reference to configuration struct
     * @param pwm Reference to PWM driver object for speed control
     */
    explicit Drv8231(const Config& config);
    // TODO: Verify if the constructor should output the PWM object

    /**
     * @brief Set the motor direction (COAST, FORWARD, REVERSE, BRAKE)
     * @param dir Direction of motor rotation
     */
    void set_direction(Direction dir);

    /**
     * @brief Set the motor speed (PWM duty cycle)
     * @param speed Motor speed as a percentage (0-100)
     */
    void set_speed(uint8_t speed);

    /**
     * @brief Get the current motor state
     * @return Current motor state
     */
    int get_state() const;

    bool init();

private:
    struct Config
    {
        Gpio& in1_pin;
        Gpio& in2_pin;
        Pwm& speed_pwm;
    };

    int state;
};

}  // namespace MM
