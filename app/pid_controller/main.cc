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
#include "pid.h"

/// NOTE: Currently running at 1KHz, which is the max sampling rate of the encoders.
/// PWM at 1KHZ as well to match the control loop frequency.

using namespace MM;

static void update_motor_drive(PID& pid, Encoder& encoder, Drv8231& motor,
                               float target_ticks_per_sec,
                               const Sample::EncoderTiming& encoder_timing)
{
    float drive = 0.0f;
    const int32_t sample_ticks =
        Sample::sample_encoder(encoder, encoder_timing);

    pid.update_ticks(sample_ticks, target_ticks_per_sec,
                     encoder_timing.sample_time_sec, drive);
    motor.set_drive(drive);
}

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();
    Motion motion(hw);

    auto [pid, target_ticks_per_sec] = get_pid_bundle();

    // Change here for different target encoder velocities.
    target_ticks_per_sec =
        400.0f;  // 400 ticks/s is approximately 1 m/s for our encoders

    hw.encoder.reset_ticks();

    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.encoder, hw.encoder_sample_us);

    while (1)
    {
        /// TEST SEQUENCE: 1. Sample encoder
        ///                2. Update generic PID with ticks/s target
        ///                3. Apply signed normalized motor drive (-1 to 1)

        update_motor_drive(pid, hw.encoder, hw.motor, target_ticks_per_sec,
                           encoder_timing);
    }

    return 0;
}
