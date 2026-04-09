#include "bno055_imu.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clk.h"
#include "board.h"
#include "nav.h"
#include <cstdint>

namespace MM
{

bool bsp_init()
{

    // initalization

    return true;
};

Board& get_board()
{
    return board;
}

}
