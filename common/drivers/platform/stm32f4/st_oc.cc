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

    // Generate update events on overflow by setting the UG bit in the TIMx_EGR

    // Set current timer_freq
    this->set_freq(init_timer_freq);
    return true;
}

bool HwOc::set_freq(uint32_t new_timer_freq)
{
    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    // Remember to enable update event after setting frequency

    // Set current timer frequency
    timer_freq = new_timer_freq;

    return true;
}

};  // namespace Stmf4
};  // namespace MM