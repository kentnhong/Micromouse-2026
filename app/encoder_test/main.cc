#include "board.h"
#include "encoder.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    /**
     * TODO: The motor has a 15:1 gear ratio and the encoder produces 12 ticks per motor shaft revolution.
     * This means there are 12 * 15 = 180 encoder ticks per output shaft revolution.
     * For RPM calculation, use instantaneous (short interval) measurements instead of averaging over a full minute.
     */
    
    // current ticks
    int32_t current_ticks = 0;
    int32_t prev_ticks = 0;

    while (1)
    {
        // loops thru the ticks
        for (int i = 0; i < 100; i++)  // About 1 second at 100Hz
        {
            current_ticks = hw.encoder.get_ticks();
        }

        // Calculate velocity
        int32_t velocity =
            (current_ticks - prev_ticks) * 100;  // ticks per second
        prev_ticks = current_ticks;

        // Calculate distance
        float distance = current_ticks * 0.01f;  // Assuming 0.01m per tick

        (void)current_ticks;
        (void)prev_ticks;
        (void)velocity;
        (void)distance;
    }

    return 0;
}
