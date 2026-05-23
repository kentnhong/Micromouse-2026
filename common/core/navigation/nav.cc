#include "nav.h"

namespace MM
{

Navigation::Navigation() : state(State::STOPPED)
{
}

Navigation::~Navigation()
{
}

void Navigation::set_mode(Floodfill::Mode m)
{
    floodfill.set_mode(m);
}

bool Navigation::is_searched()
{
    return floodfill.is_searched();
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
            if (floodfill.get_mode() == Floodfill::Mode::ZOOMING)
            {
                pending_straight_cells = floodfill.count_straight_ahead() + 1;
            }
            else
            {
                pending_straight_cells = 1;
            }
            state = State::STRAIGHT;
        }
        else if (move == 'R')
        {
            pending_straight_cells = 0;
            state = State::TURNING_RIGHT;
        }
        else if (move == 'L')
        {
            pending_straight_cells = 0;
            state = State::TURNING_LEFT;
        }
        else if (move == 'U')
        {
            pending_straight_cells = 0;
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
            motion.set_straight_cells(pending_straight_cells);
            if (motion.forward(ir))
            {
                complete();
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
