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

        hw.drv8231.get_state();  // Should return 1 (FORWARD)

        hw.drv8231.set_direction(Drv8231::Direction::REVERSE);
        hw.drv8231.set_speed(128);  // 50% speed

        hw.drv8231.get_state();  // Should return 2 (REVERSE)

        hw.drv8231.set_direction(Drv8231::Direction::BRAKE);

        hw.drv8231.get_state();  // Should return 3 (BRAKE)

        hw.drv8231.set_direction(Drv8231::Direction::COAST);
        hw.drv8231.set_speed(0);  // Stop the motor

        hw.drv8231.get_state();  // Should return 0 (COAST)
    }

    return 0;
}