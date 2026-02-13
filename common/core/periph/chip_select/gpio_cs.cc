#include "gpio_cs.h"

namespace MM
{
GpioChipSelect::GpioChipSelect(Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}

void GpioChipSelect::cs_enable()
{
    cs_pin.set(0);
}

void GpioChipSelect::cs_disable()
{
    cs_pin.set(1);
}
};  // namespace MM