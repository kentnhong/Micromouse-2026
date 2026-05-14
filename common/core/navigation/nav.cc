#include "nav.h"

namespace MM
{

Navigation::Navigation() : state(State::STOPPED)
{
}
Navigation::~Navigation()
{
}

void Navigation::update(const IrData& ir)
{
    if (at_cell_center)
    {
        // Feed current wall data
        floodfill.set_sensor_data(ir.front_wall, ir.right_wall, ir.left_wall);

        // Run the algorithm
        floodfill.update();

        // Get the next move
        char move = floodfill.get_next_move();

        // Change the robot's state based on the move
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

        at_cell_center = false;  // Decision made, now we execute it
    }

    switch (state)
    {
        case State::STRAIGHT:
            // TODO: Drive forward.

            if (/* Check if we've reached the center of the next cell using IMU data */)
            {
                at_cell_center = true;
            }
            break;

        case State::TURNING_LEFT:
            // TODO: Rotate left.
            if (/* IR sensor on the side that has no wall drive it left */)
            {
                at_cell_center = true;  // Ready for next move!
            }
            break;

        case State::TURNING_RIGHT:
            // TODO: Rotate right.
            if (/* IR sensor on the side that has no wall drive it right */)
            {
                at_cell_center = true;
            }
            break;

        case State::U_TURN:
            // TODO: Rotate 180 degrees.
            if (/* IR sensor detects the robot has turned 180 degrees */)
            {
                at_cell_center = true;
            }
            break;

        case State::STOPPED:
            // TODO: Stop motors

            break;

        case State::RESET:
            break;
    }
}

}  // namespace MM