
#include "drv8231.h"
#include <algorithm>
#include <cstdint>

namespace MM
{

static inline float clamp_drive(float drive)
{
    return std::clamp(drive, -1.0f, 1.0f);
}

Drv8231::Drv8231(Gpio& in1, Gpio& in2, Pwm& pwm)
    : in1_pin(in1), in2_pin(in2), pwm(pwm)
{
}

bool Drv8231::init()
{
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
    speed = std::clamp<uint8_t>(speed, 0, 100);
    pwm.set_duty_cycle(speed);
}

void Drv8231::set_drive(float drive)
{
    const float clamped_drive = clamp_drive(drive);

    if (clamped_drive > 0.0f)
    {
        set_direction(Direction::FORWARD);
        set_speed(static_cast<uint8_t>((clamped_drive * 100.0f) + 0.5f));
        return;
    }

    if (clamped_drive < 0.0f)
    {
        set_direction(Direction::REVERSE);
        set_speed(static_cast<uint8_t>((-clamped_drive * 100.0f) + 0.5f));
        return;
    }

    set_direction(Direction::COAST);
    set_speed(0);
}

int Drv8231::get_state() const
{
    return state;
}

}  // namespace MM
