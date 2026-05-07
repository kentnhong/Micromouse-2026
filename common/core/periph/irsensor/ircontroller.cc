#include "ircontroller.h"

namespace MM
{
IrController::IrController(IrControllerParams& params_)
    : ir_sequence{params_.ir_sequence}, ir_vals{params_.ir_vals}
{
}

bool IrController::update()
{
    bool result = true;

    // For loop through all IR Sensors
    for (size_t i = 0; i < ir_sequence.size(); i++)
    {
        while (ir_sequence[i].is_done() != true)
        {
            result = result && ir_sequence[i].update();
        }

        // TODO: Maybe implement this in a way where its reusable logic for values larger than 3
        switch (i)
        {
            case 0:
                ir_vals.left = ir_sequence[i].get_ir_val();
                break;
            case 1:
                ir_vals.front_left = ir_sequence[i].get_ir_val();
                break;
            case 2:
                ir_vals.front_right = ir_sequence[i].get_ir_val();
                break;
            case 3:
                ir_vals.right = ir_sequence[i].get_ir_val();
                break;
            default:
                ir_vals.left = 0;
                ir_vals.front_left = 0;
                ir_vals.front_right = 0;
                ir_vals.right = 0;
        }
    }
    return true;
}

IrValues& IrController::get_ir_vals() const
{
    return ir_vals;
}

}  // namespace MM