/**
* @file main.cc
* @author Bex Saw
* @brief DRV8231 motor driver test application
* @version 0.1
*/

#include "board.h"
#include "common/drivers/time/delay.h"
#include "drv8231.h"

using namespace MM;

volatile int g_dir = 0;
volatile uint8_t g_duty = 0;
volatile int g_drv_state = 0;
volatile uint32_t g_step = 0;

static void run_step(Board& hw, Drv8231::Direction dir, uint8_t duty,
                     uint32_t ms)
{
    hw.drv8231.drive(dir, duty);
    g_dir = static_cast<int>(dir);
    g_duty = duty;
    g_drv_state = hw.drv8231.get_state();
    g_step++;
    MM::Utils::delay_ms(ms);
}

int main()
{
    bsp_init();
    Board& hw = get_board();

    hw.pwm1.set_frequency(2000);
    hw.pwm2.set_frequency(2000);

    while (1)
    {
        run_step(hw, Drv8231::Direction::FORWARD, 25, 500);
        run_step(hw, Drv8231::Direction::FORWARD, 50, 500);
        run_step(hw, Drv8231::Direction::FORWARD, 75, 500);
        run_step(hw, Drv8231::Direction::COAST, 0, 300);
        run_step(hw, Drv8231::Direction::REVERSE, 25, 500);
        run_step(hw, Drv8231::Direction::REVERSE, 50, 500);
        run_step(hw, Drv8231::Direction::REVERSE, 75, 500);
    }

    return 0;
}
