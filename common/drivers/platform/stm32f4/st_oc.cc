#include "st_oc.h"

namespace
{
constexpr uint32_t kMaxPclkFreq{50000000};
constexpr uint32_t kMaxPrescaler{65536};
};  // namespace
namespace MM
{
namespace Stmf4
{
HwOc::HwOc(StOcParams params_)
    : pclk{params_.pclk}, base_addr{params_.base_addr}
{
}

bool HwOc::init(uint32_t init_timer_freq)
{
    // Select CK_INT as clock source (reset SMS in TIMx_SMCR)
    // Enable Edge-aligned mode from clearing CMS in TIMx_CR1
    // Set as upcounter by clearing DIR in TIMx_CR1
    // Enable Auto-reload preload enable bit (ARPE) in TIMx_CR1 register
    // Do not set UIF flag for interrupt and DMA requests (enable URS in TIMx_CR1)

    // Set current timer_freq
    this->set_freq(init_timer_freq);

    // Start counter (set CEN in TIMx_CR1)
    start_counter();

    return true;
}

bool HwOc::set_freq(uint32_t new_timer_freq)
{
    // Check if new_timer_freq is in valid range
    if (new_timer_freq < 1 || new_timer_freq > kMaxPclkFreq)
    {
        return false;
    }

    // Disable counter
    stop_counter();

    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    // Set new timer freq in between here
    prescaler = pclk / new_timer_freq;
    if (prescaler > kMaxPrescaler)
    {
        return false;
    }
    base_addr->PSC = static_cast<uint16_t>(prescaler - 1);

    // Remember to enable update event after setting frequency (clear UDIS)

    // Set current timer frequency
    timer_freq = new_timer_freq;

    // Enable timer counter
    start_counter();

    return true;
}

void HwOc::start_counter()
{
    base_addr->CR1 |= TIM_CR1_CEN;
}

void HwOc::stop_counter()
{
    base_addr->CR1 &= ~TIM_CR1_CEN;
}

};  // namespace Stmf4
};  // namespace MM