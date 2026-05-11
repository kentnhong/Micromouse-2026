#include "ircontroller.h"

namespace MM
{
IrController::IrController(IrControllerParams& params_)
    : ir_sequence{params_.ir_sequence}, ir_vals{params_.ir_vals}
{
}

bool IrController::update()
{
    if (sequence_done)
    {
        // Hold until consumer acknowledges completion via is_sequence_done().
        return true;
    }

    bool result = true;
    switch (current_state)
    {
        case IrControllerStates::LEFT:
            result = result && ir_sequence[0]->update();
            if (ir_sequence[0]->is_done())
            {
                ir_vals.left = ir_sequence[0]->get_ir_val();
                current_state = IrControllerStates::FRONT_LEFT;
                result = result && ir_sequence[0]->reset();
            }
            break;
        case IrControllerStates::FRONT_LEFT:
            result = result && ir_sequence[1]->update();
            if (ir_sequence[1]->is_done())
            {
                ir_vals.front_left = ir_sequence[1]->get_ir_val();
                current_state = IrControllerStates::FRONT_RIGHT;
                result = result && ir_sequence[1]->reset();
            }
            break;
        case IrControllerStates::FRONT_RIGHT:
            result = result && ir_sequence[2]->update();
            if (ir_sequence[2]->is_done())
            {
                ir_vals.front_right = ir_sequence[2]->get_ir_val();
                current_state = IrControllerStates::RIGHT;
                result = result && ir_sequence[2]->reset();
            }
            break;
        case IrControllerStates::RIGHT:
            result = result && ir_sequence[3]->update();
            if (ir_sequence[3]->is_done())
            {
                ir_vals.right = ir_sequence[3]->get_ir_val();
                result = result && ir_sequence[3]->reset();
                current_state = IrControllerStates::LEFT;
                sequence_done = true;
            }
            break;
    }
    return result;
}

IrValues& IrController::get_ir_vals()
{
    return ir_vals;
}

const IrValues& IrController::get_ir_vals() const
{
    return ir_vals;
}

bool IrController::is_sequence_done()
{
    if (!sequence_done)
    {
        return false;
    }
    sequence_done = false;
    return true;
}

}  // namespace MM