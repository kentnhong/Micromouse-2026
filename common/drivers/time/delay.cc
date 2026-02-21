#include "delay.h"

#ifdef STM32F4xx
#include "stm32f4xx.h"
#endif

namespace MM::Utils
{

uint32_t g_ms_ticks = 0;

// SysTick_Handler is defined in st_sys_clk.cc
// Provide a way for it to increment g_ms_ticks
extern "C" void IncDelayTicks(void)
{
    g_ms_ticks = g_ms_ticks + 1;
}

void DelayMs(uint32_t ms)
{
#ifdef STM32F4xx
    // Embedded target: busy-wait loop
    for (uint32_t i = 0; i < ms * 4000; i++)
    {
        __NOP();
    }
#else
    (void)ms;
#endif
}

void DelayUs(uint32_t us)
{
#ifdef STM32F4xx
    for (uint32_t i = 0; i < us * 4; i++)
    {
        __NOP();
    }
#else
    (void)us;
#endif
}

}  // namespace MM::Utils
