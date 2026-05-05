#include "irsensor.h"

namespace MM
{
IrSensor::IrSensor(IrParams params_)
    : adc{params_.adc}, emitter{params_.emitter}
{
}

bool IrSensor::init()
{
    current_state = IrStates::SAMPLE_OFF_1;

    for (auto& val : ambient)
    {
        val = 0;
    }

    for (auto& val : combined)
    {
        val = 0;
    }

    ir_val = 0;

    return emitter.set(false);
}

bool IrSensor::update()
{
    // TODO: Complete this
    switch (current_state)
    {
        case IrStates::SAMPLE_OFF_1:
            break;
        case IrStates::SAMPLE_OFF_2:
            break;
        case IrStates::EMITTER_ON:
            break;
        case IrStates::SETTLE_1:
            break;
        case IrStates::SAMPLE_ON_1:
            break;
        case IrStates::SAMPLE_ON_2:
            break;
        case IrStates::EMITTER_OFF:
            break;
        case IrStates::CALCULATE:
            break;
        default:
            // Turn off IR Emitter and reset state
            emitter.set(false);
            current_state = IrStates::SAMPLE_OFF_1;
    }

    return true;
}

uint16_t IrSensor::get_ir_val() const
{
    return ir_val;
}

void IrSensor::calculate()
{
    // Take average of 2 ambient samples
    uint16_t avg_ambient = (ambient[0] + ambient[1]) / 2u;

    // Take average of 2 combined (ambient + ir) samples
    uint16_t avg_combined = (combined[0] + combined[1]) / 2u;

    // Get rid of ambient values from the combined readings to get true IR readings
    ir_val = avg_combined - avg_ambient;

    return;
}

};  // namespace MM