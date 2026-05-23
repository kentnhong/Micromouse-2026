/**
 * @file main.cc
 * @brief Navigation app — full floodfill + motion pipeline.
 * @author Bex Saw
 */

#include "board.h"
#include "nav.h"

using namespace MM;

int main()
{
    if (!bsp_init())
    {
        return 1;
    }

    Board& hw = get_board();
    Navigation nav;

    while (1)
    {
        const IrValues& ir = hw.ir_controller.get_ir_vals();
        nav.update(ir);
        nav.execute(hw.motion_controller, ir);
    }

    return 0;
}
