
#include "drv8231.h"
#include <cstdint>

namespace MM
{
Drv8231::Drv8231(Gpio& in1, Gpio& in2, Pwm& pwm)
    : in1_pin(in1), in2_pin(in2), pwm(pwm)
{
}

bool Drv8231::init()
{
    // Set initial state to COAST
    set_direction(Direction::COAST);
    set_speed(0);

    return true;
}

void Drv8231::set_direction(Direction dir)
{
    switch (dir)
    {
        case Direction::COAST:
            in1_pin.set(0);
            in2_pin.set(0);
            break;
        case Direction::FORWARD:
            in1_pin.set(1);
            in2_pin.set(0);
            break;
        case Direction::REVERSE:
            in1_pin.set(0);
            in2_pin.set(1);
            break;
        case Direction::BRAKE:
            in1_pin.set(1);
            in2_pin.set(1);
            break;
    }
    state = static_cast<int>(dir);
}

void Drv8231::set_speed(uint8_t speed)
{
    pwm.set_duty_cycle(speed);
}

int Drv8231::get_state() const
{
    return state;
}

}  // namespace MM