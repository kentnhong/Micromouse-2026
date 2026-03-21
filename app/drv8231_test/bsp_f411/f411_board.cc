#include "drv8231.h"
#include "st_gpio.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

// Initialize GPIO and PWM objects for DRV8231

// In1 = IO

// In2 = IO

// PWM = TIM

// Create DRV8231 object

// Create Clock object for system clock configuration

// Create Board struct instance
Board board = {.drv8231 = drv8231, .in1 = in1, .in2 = in2, .pwm = pwm};

bool bsp_init()
{
    // Enable GPIO and PWM clocks

    // Initialize GPIO pins for IN1 IN2 and PWM

    // Initialize DRV8231 object

    // Configure system clock

    return true;
}

Board& get_board()
{
    return board;
}
}  // namespace MM