#include "nav.h"

namespace MM
{

/**
* @note Use IMU yaw & accel to decide when to start/stop a turn, 
* or to correct the robot’s heading during straight runs
*/

void Navigation::update(
    const Bno055Data&
        imu)  /// TODO: Still need to add the IR resolution data to this function, and use it in a state
{

    switch (state)
    {
        case State::STRAIGHT:

            break;
        case State::TURNING_LEFT:

            break;
        case State::TURNING_RIGHT:

            break;
        case State::STOPPED:

            break;
        case State::RESET:

            break;
        default:
            break;
    }
}

void Navigation::check_turn()
{
    // Analyze the IR resolution data to determine if a turn is needed, and update the state accordingly
}
}  // namespace MM