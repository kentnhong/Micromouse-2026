#include "st_oc.h"

namespace MM
{
namespace Stmf4
{
HwOc::HwOc(StOcParams params_) : pclk{params_.pclk}
{
}

bool HwOc::init(uint32_t init_timer_freq)
{

    // Enable Auto-reload preload enable bit (ARPE) in TIMx_CR1 register
    // Enable Edge-aligned mode from clearing CMS in TIMx_CR1
    // Set as upcounter by clearing DIR in TIMx_CR1
    // Do not set UIF flag for interrupt and DMA requests (enable URS in TIMx_CR1)

    // Set current timer_freq
    this->set_freq(init_timer_freq);

    // Start counter (set CEN in TIMx_CR1)

    return true;
}

bool HwOc::set_freq(uint32_t new_timer_freq)
{
    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    // Set new timer freq in between here
    // Remember to enable update event after setting frequency (clear UDIS)

    // Set current timer frequency
    timer_freq = new_timer_freq;

    return true;
}

};  // namespace Stmf4
};  // namespace MM