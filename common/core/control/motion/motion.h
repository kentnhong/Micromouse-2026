/**
* @file motion.h
* @brief PID controller sequeunces for motion control helpers
* @author Bex Saw
*/

#include "enc_sample.h"
#include "pid.h"
#include "pid_controller/board.h"
#include "trapezoidal.h"

namespace MM
{

struct MotionDebug
{
    int32_t sample_ticks = 0;
    float error = 0.0f;
    float p_term = 0.0f;
    float i_term = 0.0f;
    float d_term = 0.0f;
    float output = 0.0f;
};

class Motion
{
public:
    /**
        * @brief Constructor for the Motion class
        * @param hw Reference to the hardware interface
        */
    Motion(Board& hw, const Gains& gains);

    /**
        * @brief Updates the motion controler, should be called in a loop
        * @return true if the motion is complete, false otherwise
        */
    bool update();

    /**
        * @brief Returns last sample/PID values for external logging.
        */
    const MotionDebug& get_debug() const
    {
        return debug_data;
    }

private:
    Board& hw;
    PID pid;
    Trapezoidal profile;
    Sample::EncoderTiming encoder_timing;
    MotionDebug debug_data{};
};
}  // namespace MM