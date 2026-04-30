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
        COAST = 0u,
        FORWARD = 1u,
        REVERSE = 2u,
        BRAKE = 3u
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
     * @param speed Speed of motor rotation as a duty cycle (0-100)
     */
    void set_speed(uint8_t speed);

    /**
     * @brief Set signed normalized motor drive.
     * @param drive Motor command from -1.0 (reverse) to 1.0 (forward)
     */
    void set_drive(float drive);

    int get_state() const;

    bool init();

private:
    Gpio& in1_pin;
    Gpio& in2_pin;
    Pwm& pwm;
    int state;
};

}  // namespace MM
