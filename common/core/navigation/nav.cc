#include "nav.h"

namespace MM
{

<<<<<<< HEAD
<<<<<<< HEAD
Navigation::Navigation() : state(State::STOPPED)
{
}

Navigation::~Navigation()
{
}

void Navigation::update(const IrValues& ir)
{
    if (at_cell_center)
    {
        // Feed it into your perfectly prepared Floodfill
        floodfill.process_ir_data(ir);

        // Let Floodfill update its internal arrays
        floodfill.update();

        // Ask Floodfill "Where to next?"
        char move = floodfill.get_next_move();

        // Tell Navigation to execute the physical move
        if (move == 'F')
        {
            state = State::STRAIGHT;
        }
        else if (move == 'R')
        {
            state = State::TURNING_RIGHT;
        }
        else if (move == 'L')
        {
            state = State::TURNING_LEFT;
        }
        else if (move == 'U')
        {
            state = State::U_TURN;
        }

        at_cell_center = false;
    }
}

void Navigation::execute(MotionController& motion, const IrValues& ir)
{
    // The Execution State Machine
    switch (get_current_state())
    {
        case State::STRAIGHT:
            if (motion.forward(ir))
            {
                complete();  // Done! tell Nav to make next decision
            }
            break;

        case State::TURNING_LEFT:
            if (motion.turn_left())
            {
                complete();
            }
            break;

        case State::TURNING_RIGHT:
            if (motion.turn_right())
            {
                complete();
            }
            break;

        case State::U_TURN:
            if (motion.u_turn())
            {
                complete();
            }
            break;

        case State::STOPPED:
            motion.stop();
            break;

        case State::RESET:
=======
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
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
            break;
    }
}

<<<<<<< HEAD
}  // namespace MM
=======
} // namespace MM
>>>>>>> ea30492 (navigation)
=======
void Navigation::check_turn()
{
    // Analyze the IR resolution data to determine if a turn is needed, and update the state accordingly
}
}  // namespace MM
>>>>>>> 73a34e4 (structure the state machine for the navigational control)
