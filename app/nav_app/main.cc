#include "board.h"
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
    }
}