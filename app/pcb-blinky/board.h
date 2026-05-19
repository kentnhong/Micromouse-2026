#pragma once
#include "../../common/drivers/io/gpio.h"

namespace MM
{

struct Board
{
    Gpio& led_a;
    Gpio& led_b;
    Gpio& led_c;
};

bool board_init(void);
Board& get_board(void);

}  // namespace MM