/**
 * @file main.cc
 * @brief CLI app — button-triggered search and zoom runs with full nav+motion pipeline.
 * @author Bex Saw
 */

#include "board.h"
#include "cli.h"
#include "nav.h"

using namespace MM;

int main()
{
    if (!board_init())
    {
        return 1;
    }

    Board& hw = get_board();

    Navigation nav;

    Cli cli{CliParams{.led1 = hw.led1,
                      .led2 = hw.led2,
                      .led3 = hw.led3,
                      .search_bt = hw.search_bt,
                      .zoom_bt = hw.zoom_bt,
                      .nav = nav,
                      .motion = hw.motion_controller,
                      .ir_controller = &hw.ir_controller}};

    while (1)
    {
        cli.update();
    }

    return 0;
}
