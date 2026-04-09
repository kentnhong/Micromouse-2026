<<<<<<< HEAD
#include <cstdint>
#include "bno055_imu.h"
#include "board.h"
#include "nav.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clk.h"
=======
#include "bno055_imu.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clk.h"
#include "board.h"
#include "nav.h"
#include <cstdint>
>>>>>>> ea30492 (navigation)

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

<<<<<<< HEAD
}  // namespace MM
=======
}
>>>>>>> ea30492 (navigation)
