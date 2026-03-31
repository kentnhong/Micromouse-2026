#include <cstdlib>
#include "board.h"
#include "pwm.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    while (1)
    {
        /* You don't need to change the set_frequency() because as the motor speed changes, the frequency will remain 
         constant and the duty cycle will adjust to maintain the desired speed. You can experiment with different 
         frequencies and duty cycles to see how they affect the motor's behavior. */

        hw.pwm.set_duty_cycle(25);

        for (volatile size_t i = 0; i < 4000000; i++);

        hw.pwm.set_duty_cycle(90);

        for (volatile size_t i = 0; i < 4000000; i++);

        hw.pwm.set_duty_cycle(42);

        for (volatile size_t i = 0; i < 4000000; i++);
    }

    return 0;
}