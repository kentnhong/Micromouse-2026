/**
* @file motion.h
* @brief PID controller sequeunces for motion control helpers
* @author Bex Saw
*/

#include <numbers>
#include <tuple>
#include "enc_sample.h"
#include "pid.h"
#include "pid_controller/board.h"

namespace MM
{
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

private:
    Board& hw;
    PID pid;
};
}  // namespace MM
