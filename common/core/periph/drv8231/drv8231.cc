
#include "drv8231.h"
#include <algorithm>
#include <cstdint>

namespace MM
{

static inline float clamp_drive(float drive)
{
    return std::clamp(drive, 0.0f, 1.0f);
}

Drv8231::Drv8231(Gpio& in1, Gpio& in2, Pwm& speed)
    : in1(in1), in2(in2), speed(speed)
{
}

bool Drv8231::init()
{
    drive(Direction::COAST, 0);

    return true;
}

void Drv8231::drive(Direction dir, uint8_t duty_cycle)
{
    switch (dir)
    {
        case Direction::COAST:
            in1.set(0);
            in2.set(0);
            break;
        case Direction::FORWARD:
            in1.set(1);
            in2.set(0);
            break;
        case Direction::REVERSE:
            in1.set(0);
            in2.set(1);
            break;
        case Direction::BRAKE:
            in1.set(1);
            in2.set(1);
            break;
    }
    state = static_cast<int>(dir);
    speed.set_duty_cycle(std::clamp<uint8_t>(duty_cycle, 0, 100));
}

int Drv8231::get_state() const
{
    return state;
}

}  // namespace MM
