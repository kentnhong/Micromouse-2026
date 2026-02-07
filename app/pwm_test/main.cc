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
        hw.pwm.setFrequency(10000);
        hw.pwm.setDutyCycle(25);

        for (volatile size_t i = 0; i < 4000000; i++);

        hw.pwm.setFrequency(1000);
        hw.pwm.setDutyCycle(90);

        for (volatile size_t i = 0; i < 4000000; i++);

        hw.pwm.setFrequency(20000);
        hw.pwm.setDutyCycle(42);

        for (volatile size_t i = 0; i < 4000000; i++);
    }

    return 0;
}