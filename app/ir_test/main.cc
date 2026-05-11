#include <array>
#include "board.h"
#include "delay.h"
using namespace MM;

int main(int argc, char* argv[])
{
    [[maybe_unused]] bool result = true;

    result &= board_init();
    Board& board = get_board();

    while (1);
    return 0;
}