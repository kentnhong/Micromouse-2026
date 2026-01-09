
#pragma once
#include "i2c.h"

namespace MM
{

struct Board
{
    I2c& i2c;
};

bool bsp_init(void);
Board& get_board(void);

}  // namespace MM