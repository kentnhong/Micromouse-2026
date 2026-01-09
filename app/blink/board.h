#pragma once
#include "../../common/drivers/io/gpio.h"

namespace MM
{

struct Board
{
    Gpio& led;
};

bool board_init(void);
Board& get_board(void);

}  // namespace MM