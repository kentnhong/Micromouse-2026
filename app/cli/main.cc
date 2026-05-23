/**
* @file main.cc
* @author Bex Saw
* @brief Main entry point for the CLI application.
* @version 1.0
*/
#include "board.h"

using namespace MM;

int main()
{
    if (!board_init())
    {
        return 1;
    }

    Board& hw = get_board();

    while (1)
    {
        hw.cli.update();
    }

    return 0;
}
