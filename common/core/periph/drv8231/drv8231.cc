
#include "drv8231.h"
#include <algorithm>
#include <cstdint>

namespace MM
{

static inline float clamp_drive(float drive)
{
    return std::clamp(drive, 0.0f, 1.0f);
}

Drv8231::Drv8231(const Config& config, const Pwm& pwm)
    : in1_pin(config.in1_pin),
      in2_pin(config.in2_pin),
      speed_pwm(config.speed_pwm)
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

int Drv8231::get_state() const
{
    return state;
}

}  // namespace MM
