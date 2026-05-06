#include "st_timebase.h"

namespace
{
constexpr uint32_t kMaxPrescaler{65536};
constexpr uint32_t kArrMax16{65535};
constexpr uint32_t kArrMax32{4294967295};
constexpr uint32_t kMicrosecondsConversion{1000000};
};  // namespace
namespace MM
{
namespace Stmf4
{
HwTimebase::HwTimebase(StTimebaseParams params_) : base_addr{params_.base_addr}
{
}

bool HwTimebase::set_freq(uint32_t new_timer_freq, uint32_t pclk)
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
        stop();
    }

    // Set new timer freq in between here
    prescaler = pclk / new_timer_freq;
    if (prescaler > kMaxPrescaler)
    {
        if (was_running)
        {
            start();
        }
        return false;
    }
    base_addr->PSC = static_cast<uint16_t>(prescaler - 1);

    // Set the achievable counter clock after integer division.
    timer_freq = pclk / prescaler;

    if (was_running)
    {
        start();
    }

    return true;
}

bool HwTimebase::set_period_us(std::chrono::microseconds period_us)
{
    if (timer_freq == 0 || period_us <= std::chrono::microseconds::zero())
    {
        return false;
    }

    const uint64_t period_us_count = static_cast<uint64_t>(period_us.count());
    const uint64_t ticks =
        (static_cast<uint64_t>(timer_freq) * period_us_count) /
        kMicrosecondsConversion;
    if (base_addr == TIM1 || base_addr == TIM3 || base_addr == TIM4)
    {
        if (ticks == 0 || ticks > (static_cast<uint64_t>(kArrMax16) + 1ULL))
        {
            return false;
        }
    }
    else if (base_addr == TIM2 || base_addr == TIM5)
    {
        if (ticks == 0 || ticks > (static_cast<uint64_t>(kArrMax32) + 1ULL))
        {
            return false;
        }
    }

    // Disable update event so it doesn't conflict with auto reload val being written to shadow register (set UDIS in TIMx_CR1)
    base_addr->CR1 |= TIM_CR1_UDIS;

    // Set period here
    uint32_t reload = static_cast<uint32_t>(ticks - 1ULL);
    if (base_addr == TIM1 || base_addr == TIM3 || base_addr == TIM4)
    {
        base_addr->ARR = static_cast<uint16_t>(reload);
    }
    else if (base_addr == TIM2 || base_addr == TIM5)
    {
        base_addr->ARR = reload;
    }

    // Remember to enable update event after setting frequency (clear UDIS)
    base_addr->CR1 &= ~TIM_CR1_UDIS;

    period_ticks = reload;

    return true;
}

void HwTimebase::start()
{
    base_addr->CR1 |= TIM_CR1_CEN;
}

void HwTimebase::stop()
{
    base_addr->CR1 &= ~TIM_CR1_CEN;
}

uint32_t HwTimebase::get_count() const
{
    return base_addr->CNT;
}

uint32_t HwTimebase::get_freq() const
{
    return timer_freq;
}

uint32_t HwTimebase::get_max_count() const
{
    if (base_addr == TIM1 || base_addr == TIM3 || base_addr == TIM4)
    {
        return UINT16_MAX;
    }
    else if (base_addr == TIM2 || base_addr == TIM5)
    {
        return UINT32_MAX;
    }

    return 0;
}

};  // namespace Stmf4
};  // namespace MM