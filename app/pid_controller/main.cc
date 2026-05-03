/**
* @file main.cc
* @brief PID controller main application
* @author Bex Saw
* @date 3/31/2026
*/

#include <numbers>
#include <tuple>
#include "board.h"
#include "enc_sample.h"
#include "motion.h"

/// NOTE: Currently running at 1KHz, which is the max sampling rate of the encoders.
/// PWM at 1KHZ as well to match the control loop frequency.

using namespace MM;

volatile bool g_control_tick = false;

/// NOTE: sample_encoder waits 1000 us -> PID runs about 1 kHz
///       with TIM4 generating an interrupt every 1000 us to set the control tick flag.

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();

    constexpr Gains kVelocityGains{0.1f, 0.0f, 0.0f};
    Motion motion(hw, kVelocityGains);

    hw.encoder.reset_ticks();

    /** 
    * @note Scale for control:
    * -1.0 = full reverse 
    * 0.0 = stop
    * 1.0 = full forward
    * 
    * @note final_drive = 0.25  -> duty_cycle = 25
    *       final_drive = 0.70  -> duty_cycle = 70
    *       final_drive = -0.40 -> direction reverse, duty_cycle = 40
    */

    uint32_t last_ms = Utils::get_ms_ticks();
    uint32_t count = 0;

    while (1)
    {
        motion.update();
    }

    return 0;
}

/**
* @note 
*   -> Motion::update()
*   -> sample encoder ticks
*   -> target 100 mm/s converted to ticks/s
*   -> PID::update()
*   -> measured ticks converted to ticks/s
*   -> error = target velocity - measured velocity
*   -> PID computes -1.0 to 1.0 output
*   -> output sign becomes direction (FORWARD or REVERSE)
*   -> output magnitude becomes PWM duty % 0-100
*   -> motor.drive(direction, duty)
*/
