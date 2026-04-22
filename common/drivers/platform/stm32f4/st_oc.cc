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

    if (!this->set_freq(init_timer_freq))
    {
        return false;
    }

    // Start counter (set CEN in TIMx_CR1)
    start_counter();

    return true;
}

bool HwOc::set_freq(uint32_t new_timer_freq)
{
    // Check if new_timer_freq is in valid range
    if (new_timer_freq == 0 || new_timer_freq > pclk)
    {
        return false;
    }

    const bool was_running = (base_addr->CR1 & TIM_CR1_CEN) != 0U;
    if (was_running)
    {
        // Disable counter while changing the prescaler.
        stop_counter();
    }

    // Set new timer freq in between here
    prescaler = pclk / new_timer_freq;
    if (prescaler > kMaxPrescaler)
    {
        if (was_running)
        {
            start_counter();
        }
        return false;
    }
    base_addr->PSC = static_cast<uint16_t>(prescaler - 1);

    // Set the achievable counter clock after integer division.
    timer_freq = pclk / prescaler;

    if (was_running)
    {
        start_counter();
    }

    return true;
}

bool HwOc::set_period(std::chrono::microseconds period_us)
{
    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    // Set period here
    // Remember to enable update event after setting frequency (clear UDIS)
    return true;
}

bool HwOc::set_compare(std::chrono::microseconds compare_us)
{
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