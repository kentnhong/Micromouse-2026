#pragma once
#include "gpio_cs.h"
#include "spi.h"
#include "stm32f4xx.h"

//do the cmake

namespace MM
{

struct Board
{
    GpioChipSelect& cs;
    Spi& spi1;
};

bool BSP_Init();
Board& Get_Board();

}  // namespace MM