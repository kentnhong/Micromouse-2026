/**
* @file main.cc
* @author Bex Saw
* @brief DRV8231 motor driver test application
* @version 0.1
*/

#include "board.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    while (1)
    {
        // Test motor sequences (FORWARD -> REVERSE -> BRAKE -> COAST)
        hw.drv8231.set_direction(Drv8231::Direction::FORWARD);
        hw.drv8231.set_speed(128);  // 50% speed

        Utils::DelayMs(2000);  // Run forward for 2 seconds

        hw.drv8231.set_direction(Drv8231::Direction::REVERSE);
        hw.drv8231.set_speed(128);  // 50% speed

        Utils::DelayMs(2000);  // Run reverse for 2 seconds

        hw.drv8231.set_direction(Drv8231::Direction::BRAKE);

        Utils::DelayMs(1000);  // Brake for 1 second

        hw.drv8231.set_direction(Drv8231::Direction::COAST);
        hw.drv8231.set_speed(0);  // Stop the motor

        Utils::DelayMs(2000);  // Coast for 2 seconds
    }

    return 0;
}