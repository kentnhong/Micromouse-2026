#include "delay.h"

#ifdef STM32F4xx
#include "stm32f4xx.h"
#endif

namespace MM::Utils
{
namespace
{
const MM::Timebase* g_tb = nullptr;
static constexpr uint32_t kDelayFreq{1'000'000};
static constexpr uint32_t kMsMax{4'294'967};
}  // namespace

volatile uint32_t g_ms_ticks = 0;

// SysTick_Handler is defined in st_sys_clk.cc
// Provide a way for it to increment g_ms_ticks
extern "C" void IncDelayTicks(void)
{
    g_ms_ticks = g_ms_ticks + 1;
}

bool bind_timebase(const Timebase& timebase)
{
    if (timebase.get_max_count() != UINT32_MAX)
    {
        return false;
    }
    if (timebase.get_freq() != kDelayFreq)
    {
        return false;
    }
    g_tb = &timebase;
    return true;
}

bool delay_ms(uint32_t ms)
{
    if (g_tb == nullptr)
    {
        return false;
    }
    if (ms > kMsMax)
    {
        return false;
    }

    // Convert ms to us
    uint32_t us = ms * 1000u;

    uint32_t start = g_tb->get_count();
    while ((g_tb->get_count() - start) < us);

    return true;
}

bool delay_us(uint32_t us)
{
    if (g_tb == nullptr)
    {
        return false;
    }

    uint32_t start = g_tb->get_count();
    while ((g_tb->get_count() - start) < us);

    return true;
}

uint32_t get_ms_ticks()
{
    return g_ms_ticks;
}

}  // namespace MM::Utils
