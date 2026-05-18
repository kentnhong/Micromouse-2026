/**
* @file main.cc
* @author Bex Saw
* @brief Main entry point for the CLI application.
* @version 1.0
*/
#include "board.h"

using namespace MM;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();
    while (1)
    {
        // cli function
    }

    return 0;
}
