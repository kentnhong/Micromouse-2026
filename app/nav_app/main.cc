#include "board.h"
<<<<<<< HEAD
#include "ircontroller.h"
#include "motioncontroller.h"
#include "nav.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    Navigation nav;
    MotionController motion;  // Motor API

    while (1)
    {
        const IrValues& sensor_data = hw.ir_controller.get_ir_vals();
        nav.update(sensor_data);
        nav.execute(motion, sensor_data);
=======

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
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
    }
}