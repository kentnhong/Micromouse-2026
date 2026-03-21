/**
* @file main.cc
* @author Bex Saw
* @brief DRV8231 motor driver test application
* @version 0.1
*/

#include "board.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    while (1)
    {
        // Test motor sequences (FORWARD -> REVERSE -> BRAKE -> COAST)
    }

    return 0;
}