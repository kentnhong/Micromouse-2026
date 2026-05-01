#include "irsensor.h"

namespace MM
{
IrSensor::IrSensor(IrParams params_)
    : adc{params_.adc}, dma{params_.dma}, emitter{params_.emitter}
{
}

bool IrSensor::update()
{
    // TODO: Complete this
    return true;
}

};  // namespace MM