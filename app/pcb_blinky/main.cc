#include <cstdlib>
#include "board.h"
#include "delay.h"
#include "gpio.h"
#include "st_timebase.h"

using namespace MM;

int main(int argc, char* argv[])
{
    board_init();
    Board hw = get_board();

    // Create timebase object using TIM5
    StTimebaseParams tb_params = {TIM5};
    Stmf4::HwTimebase timebase(tb_params);

    // Enable clock for TIM5
    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;

    // Initialize timebase (50MHz PCLK, 1MHz timer frequency)
    timebase.init(50'000'000, 1'000'000, std::chrono::microseconds(0xFFFFFFFF));
    timebase.start();

    // Bind timebase to use delay functions
    Utils::bind_timebase(timebase);

    while (1)
    {
        hw.led_a.toggle();
        MM::Utils::delay_ms(500);

        hw.led_b.toggle();
        MM::Utils::delay_ms(500);

        hw.led_c.toggle();
        MM::Utils::delay_ms(500);
    }

    return 0;
}
