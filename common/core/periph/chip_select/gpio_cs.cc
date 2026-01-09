#include "gpio_cs.h"

MM::GpioChipSelect::GpioChipSelect(MM::Gpio& cs_pin_) : cs_pin{cs_pin_}
{
}

void MM::GpioChipSelect::ChipSelectEnable()
{
    cs_pin.set(0);
}

void MM::GpioChipSelect::ChipSelectDisable()
{
    cs_pin.set(1);
}
