#include "st_oc.h"

namespace MM
{
namespace Stmf4
{
HwOc::HwOc(StOcParams params_)
    : timer_freq{params_.timer_freq}, pclk{params_.pclk}
{
}
};  // namespace Stmf4
};  // namespace MM