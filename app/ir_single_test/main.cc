#include "board.h"

using namespace MM;

volatile uint16_t g_raw = 0;
volatile uint32_t g_sample_n = 0;

int main()
{
    board_init();
    Board& hw = get_board();

    hw.ir_led.set(1);

    uint16_t buf = 0;

    while (1)
    {
        hw.dma.arm_p2m(reinterpret_cast<uintptr_t>(&buf), 1);
        hw.dma.start();
        hw.adc.convert(true, 1);

        while (!hw.dma.complete())
        {
        }

        g_raw = buf;
        g_sample_n++;
    }

    return 0;
}
