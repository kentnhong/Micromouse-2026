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
    constexpr int32_t MAX_TICKS = 1000;
    while (1)
    {
        // Test encoder getting ticks and resetting ticks
        current_ticks = hw.encoder.get_ticks();
        (void)current_ticks;

        // Busy wait
        for (volatile uint32_t i = 0; i < 1000000; i++);

        (void)prev_ticks;
        if (current_ticks > MAX_TICKS)
        {
            hw.encoder.reset_ticks();
        }
    }

    return 0;
}