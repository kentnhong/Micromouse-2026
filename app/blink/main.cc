#include <cstdlib>
#include "board.h"
#include "gpio.h"

using namespace MM;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();
    while (1)
    {
        hw.led.toggle();
    }

    return 0;
}
