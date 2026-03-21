
#include "drv8231.h"
#include <cstdint>

namespace MM
{
Drv8231::Drv8231(Gpio& in1, Gpio& in2, Pwm& pwm)
    : in1_pin(in1), in2_pin(in2), pwm(pwm)
{
}

void Drv8231::set_direction(Direction dir)
{
    switch (dir)
    {
        case Direction::COAST:
            in1_pin.set_low();
            in2_pin.set_low();
            break;
        case Direction::FORWARD:
            in1_pin.set_high();
            in2_pin.set_low();
            break;
        case Direction::REVERSE:
            in1_pin.set_low();
            in2_pin.set_high();
            break;
        case Direction::BRAKE:
            in1_pin.set_high();
            in2_pin.set_high();
            break;
    }
}

void Drv8231::set_speed(uint8_t speed)
{
    pwm.set_duty_cycle(speed);
}

}  // namespace MM