/**
* @file main.cc
* @author Bex Saw
* @brief DRV8231 motor driver test application
* @version 0.1
*/

#include "board.h"
#include "drv8231.h"
using namespace MM;

int main()
{
    bsp_init();
    Board& hw = get_board();

    // The DRV8231 needs a 1kHz PWM frequency to work properly
    hw.pwm1.set_frequency(1000);
    hw.pwm2.set_frequency(1000);

    while (1)
    {
        // 1. FORWARD at medium speed
        hw.drv8231.drive(Drv8231::Direction::FORWARD, 50);
        for (volatile size_t i = 0; i < 4000000; i++);

        // 2. COAST (Let the motor drift to a stop)
        hw.drv8231.drive(Drv8231::Direction::COAST, 0);
        for (volatile size_t i = 0; i < 4000000; i++);

        // 3. REVERSE at medium speed
        hw.drv8231.drive(Drv8231::Direction::REVERSE, 50);
        for (volatile size_t i = 0; i < 4000000; i++);

        // 4. BRAKE (Actively short the motor terminals to heavily stop it)
        hw.drv8231.drive(Drv8231::Direction::BRAKE, 0);
        for (volatile size_t i = 0; i < 4000000; i++);
    }

    return 0;
}
