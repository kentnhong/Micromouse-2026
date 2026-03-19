#include "board.h"
#include "encoder.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    // current ticks
    int32_t current_ticks = 0;
    int32_t prev_ticks = 0;
    while (1)
    {
        // Test encoder getting ticks and resetting ticks
        current_ticks = hw.encoder.get_ticks();
        (void)current_ticks;
        (void)prev_ticks;
    }

    return 0;
}