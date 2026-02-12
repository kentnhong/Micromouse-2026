#include "st_adc.h"

namespace MM
{
namespace Stmf4
{
HwAdc::HwAdc(StAdcParams& params_)
    : settings{params_.settings}, base_addr{params_.base_addr}
{
}

bool HwAdc::init()
{
    // Turn on ADC
    // Start conversion
}
};  // namespace Stmf4
};  // namespace MM