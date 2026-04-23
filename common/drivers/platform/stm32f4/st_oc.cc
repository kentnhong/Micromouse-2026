#include "st_oc.h"

namespace
{
constexpr uint32_t kMaxPclkFreq{50000000};
constexpr uint32_t kMaxPrescaler{65536};
constexpr uint32_t kArrMax{65535};
constexpr uint32_t kMicrosecondsConversion{1000000};
};  // namespace
namespace MM
{
namespace Stmf4
{
HwOc::HwOc(StOcParams params_)
    : pclk{params_.pclk}, base_addr{params_.base_addr}
{
}

bool HwOc::init()
{
    // Select CK_INT as clock source (reset SMS in TIMx_SMCR)
    // Enable Edge-aligned mode from clearing CMS in TIMx_CR1
    // Set as upcounter by clearing DIR in TIMx_CR1
    // Enable Auto-reload preload enable bit (ARPE) in TIMx_CR1 register
    // Do not set UIF flag for interrupt and DMA requests (enable URS in TIMx_CR1)

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

    if (!has_freq)
    {
        has_freq = true;
    }

    return true;
}

bool HwOc::set_period_us(std::chrono::microseconds period_us)
{
    // Check if period is in valid range (not exceeding minimum or maximum)
    // Tmax_us = ((ARRmax + 1) / timer_freq) * 1e6
    const uint64_t period_max_us_count =
        ((static_cast<uint64_t>(kArrMax) + 1ULL) * kMicrosecondsConversion) /
        timer_freq;
    const auto period_max_us = std::chrono::microseconds{period_max_us_count};

    if (period_us <= std::chrono::microseconds::zero() ||
        period_us > period_max_us)
    {
        return false;
    }

    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    // Set period here
    // Remember to enable update event after setting frequency (clear UDIS)

    if (!has_period)
    {
        has_period = true;
    }

    return true;
}

bool HwOc::set_compare_us(std::chrono::microseconds compare_us)
{
    // Check if compare is in valid range (not exceeding minimum or maximum)

    if (!has_compare)
    {
        has_compare = true;
    }
    return true;
}

bool HwOc::start_counter()
{
    if (has_freq && has_period && has_compare)
    {
        base_addr->CR1 |= TIM_CR1_CEN;
        return true;
    }
    return false;
}

void HwOc::stop_counter()
{
    base_addr->CR1 &= ~TIM_CR1_CEN;
}

};  // namespace Stmf4
};  // namespace MM