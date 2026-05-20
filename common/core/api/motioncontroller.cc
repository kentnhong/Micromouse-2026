#include "motioncontroller.h"

namespace MM
{
// TODO: Implement the motion controller methods to control the robot's movements based on the navigation state and IR sensor data.

bool MotionController::forward(const IrValues& ir)
{
    // PSEUDOCODE:
    // 1. Read encoder feedback and current wall-centering error from ir.
    // 2. Apply the trapezoidal profile using straight_cells_remaining.
    // 3. Feed the PID controller the speed target and lateral correction.
    // 4. Drive the motors until one cell has been completed.
    // 5. Return true when the next navigation decision should be made.
    //
    // Fill in the blank sections here once the profile and PID wiring exist.
    (void)ir;
    return false;
}

void MotionController::set_straight_cells(int cells)
{
    // Keep this as a simple state handoff from Navigation.
    straight_cells_remaining = cells;
}

bool MotionController::turn_left()
{

    return false;
}

bool MotionController::turn_right()
{
    return false;
}

bool MotionController::u_turn()
{
    return false;
}

void MotionController::stop()
{
}

}  // namespace MM