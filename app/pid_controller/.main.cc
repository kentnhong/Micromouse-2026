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
// #include "trapezoidal.h"

/// NOTE: Currently running at 1KHz, which is the max sampling rate of the encoders.
/// PWM at 1KHZ as well to match the control loop frequency (same thing as encoder sampling rate).

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board& hw = get_board();
    Motion motion(hw);

    auto [left_pid, right_pid, left_target_ticks_per_sec,
          right_target_ticks_per_sec] = get_pid_bundle();

    /// Trapezoidal profile;

    float left_drive = 0.0f;
    float right_drive = 0.0f;

    // Change here for different target encoder velocities.
    left_target_ticks_per_sec = 400.0f;
    right_target_ticks_per_sec = 400.0f;

    hw.left_encoder.reset_ticks();
    hw.right_encoder.reset_ticks();

    const Sample::EncoderTiming encoder_timing =
        Sample::init_encoder_timing(hw.left_encoder, hw.encoder_sample_us);

    while (1)
    {
        /// TEST SEQUENCE: 1. Sample encoders
        ///                2. Update each generic PID with ticks/s target and measured encoder ticks
        ///                3. Apply signed normalized motor drive (-1 to 1)

        EncoderInput sample_encoder = Sample::sample_encoders(
            hw.left_encoder, hw.right_encoder, encoder_timing);

        float dt_sec = encoder_timing.sample_time_sec;
        left_pid.update_ticks(sample_encoder.left_ticks,
                              left_target_ticks_per_sec, dt_sec, left_drive);
        right_pid.update_ticks(sample_encoder.right_ticks,
                               right_target_ticks_per_sec, dt_sec, right_drive);

        hw.left_motor.set_drive(left_drive);
        hw.right_motor.set_drive(right_drive);
    }

    return 0;
}
