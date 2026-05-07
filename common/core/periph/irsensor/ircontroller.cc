#include "ircontroller.h"

namespace MM
{
IrController::IrController(IrControllerParams& params_)
    : ir_sequence{params_.ir_sequence}
{
}
}  // namespace MM