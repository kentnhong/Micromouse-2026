#include "nav.h"

namespace MM
{

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
        switch (floodfill.get_mode())
        {
            case Floodfill::Mode::SEARCH:
                // Feed it into your perfectly prepared Floodfill
                floodfill.process_ir_data(ir);

                // Let Floodfill update its internal arrays
                floodfill.update();
                break;

            case Floodfill::Mode::ZOOMING:
                break;
        }

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
            break;
    }
}

}  // namespace MM
