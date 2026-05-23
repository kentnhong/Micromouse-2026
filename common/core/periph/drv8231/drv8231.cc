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
    : state{static_cast<int>(Direction::COAST)},
      in1{&in1},
      in2{&in2},
      speed{&speed},
      pwm1{nullptr},
      pwm2{nullptr}
{
}

Drv8231::Drv8231(Pwm& pwm1, Pwm& pwm2)
    : state{static_cast<int>(Direction::COAST)},
      in1{nullptr},
      in2{nullptr},
      speed{nullptr},
      pwm1{&pwm1},
      pwm2{&pwm2}
{
}

bool Drv8231::init()
{
    drive(Direction::COAST, 0);

    return true;
}

void Drv8231::drive(Direction dir, uint8_t duty_cycle)
{
    duty_cycle = std::clamp<uint8_t>(duty_cycle, 0, 100);

    switch (dir)
    {
        case Direction::COAST:
            pwm1->set_duty_cycle(0);
            pwm2->set_duty_cycle(0);
            break;
        case Direction::FORWARD:
            pwm2->set_duty_cycle(0);
            pwm1->set_duty_cycle(duty_cycle);
            break;
        case Direction::REVERSE:
            pwm1->set_duty_cycle(0);
            pwm2->set_duty_cycle(duty_cycle);
            break;
        case Direction::BRAKE:
            pwm1->set_duty_cycle(duty_cycle);
            pwm2->set_duty_cycle(duty_cycle);
            break;
    }
    state = static_cast<int>(dir);

    return;
}

int Drv8231::get_state() const
{
    return state;
}

}  // namespace MM