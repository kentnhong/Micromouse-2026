#include "board.h"

using namespace MM;

int main(int argc, char* char[])
{
    bsp_init();
    Board hw = get_board();
    while (1)
    {
        // get the latest IMU data, real time IR sensor resolution data, and update the navigation system
        Bno055Data imu_data = hw.imu.read();
        Navigation nav;
        // IR content refernce over here

        nav.check_turn();
        // update the navigation system with the latest data
        nav.update(imu_data);
    }
}