#include <stdint.h>
#include "board.h"
#include "st_sys_clk.h"

namespace MM
{

// GPIO settings for the led 

bool board_init()
{
    // init all periph
    return true;    
}

Board& get_board(void)
{
    return board;
}

}  // namespace MM
