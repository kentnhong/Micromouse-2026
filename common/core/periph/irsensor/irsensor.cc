#include "irsensor.h"

namespace MM
{
IrSensor::IrSensor(IrParams params_)
    : adc{params_.adc}, dma{params_.dma}, emitter{params_.emitter}
{
}

bool IrSensor::reset()
{
    bool result = true;
    result = result && emitter.set(false);
    current_state = IrStates::SAMPLE_OFF_1;
    done = false;
    return result;
}

bool IrSensor::update()
{
    bool result = true;

    switch (current_state)
    {
        case IrStates::SAMPLE_OFF_1:
            if (dma.complete())
            {
                result =
                    result &&
                    dma.arm_p2m(reinterpret_cast<uintptr_t>(ambient.data()), 2);
                result = result && adc.convert(true, 1);
                current_state = IrStates::SAMPLE_OFF_2;
            }
            break;
        case IrStates::SAMPLE_OFF_2:
            result = result && adc.convert(true, 1);
            current_state = IrStates::EMITTER_ON;
            break;
        case IrStates::EMITTER_ON:
            result = result && emitter.set(1);
            current_state = IrStates::SETTLE;
            break;
        case IrStates::SETTLE:
            current_state = IrStates::SAMPLE_ON_1;
            break;
        case IrStates::SAMPLE_ON_1:
            if (dma.complete())
            {
                result = result &&
                         dma.arm_p2m(
                             reinterpret_cast<uintptr_t>(combined.data()), 2);
                result = result && adc.convert(true, 1);
                current_state = IrStates::SAMPLE_ON_2;
            }
            break;
        case IrStates::SAMPLE_ON_2:
            result = result && adc.convert(true, 1);
            current_state = IrStates::EMITTER_OFF;
            break;
        case IrStates::EMITTER_OFF:
            result = result && emitter.set(0);
            current_state = IrStates::CALCULATE;
            break;
        case IrStates::CALCULATE:
            calculate();
            done = true;
            break;
    }

    return result;
}

uint16_t IrSensor::get_ir_val() const
{
    return ir_val;
}

IrStates IrSensor::get_state() const
{
    return current_state;
}

bool IrSensor::is_done() const
{
    return done;
}

void IrSensor::calculate()
{
    // Take average of 2 ambient samples
    uint16_t avg_ambient = (ambient[0] + ambient[1]) / 2u;

    // Take average of 2 combined (ambient + ir) samples
    uint16_t avg_combined = (combined[0] + combined[1]) / 2u;

    // Get rid of ambient values from the combined readings to get true IR readings
    ir_val = (avg_combined > avg_ambient) ? (avg_combined - avg_ambient) : 0;

    return;
}

};  // namespace MM