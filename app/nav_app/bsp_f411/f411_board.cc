<<<<<<< HEAD
<<<<<<< HEAD
#include <cstdint>
#include "bno055_imu.h"
#include "board.h"
#include "nav.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clk.h"
=======
=======
#include <cstdint>
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
#include "bno055_imu.h"
#include "board.h"
#include "nav.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_sys_clk.h"
<<<<<<< HEAD
#include "board.h"
#include "nav.h"
#include <cstdint>
>>>>>>> ea30492 (navigation)
=======
>>>>>>> 73a34e4 (structure the state machine for the navigational control)

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
<<<<<<< HEAD
}  // namespace MM
=======
}
>>>>>>> ea30492 (navigation)
=======
}  // namespace MM
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
