#pragma once

// NOTE: Will things a bit differently I will have the class for the BSP init with a class so it's easier to manage different things

/*

// bsp.h
#include "pid_controller_bsp.h"
#include "hw_input_ir_bsp.h"
#include "hc05_bsp.h"
#include "other_gpio_bsp.h"
#include etc....

class BSP {
public:
    Controls controls;
    PIDController pid;
    IR ir;
    Comm comm;
    GPIO gpio;

    void board_init() 
    {
        pid.init();
        ir.init();
        comm.init();
        gpio.init();
    }
};
*/

namespace MM
{

struct Board
{
    Gpio& led;
};

bool board_init(void);
Board& get_board(void);

}  // namespace MM