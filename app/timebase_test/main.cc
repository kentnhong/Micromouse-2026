/**
 * @file main.cc
 * @author Kent Hong
 * @brief Timebase driver test app
 */
#include "board.h"

using namespace MM;

int main()
{
    board_init();
    Board& board = get_board();
    board.counter.start();

    while (1)
    {
    }
}