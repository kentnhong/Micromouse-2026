#include <numbers>
#include "board.h"
#include "delay.h"
#include "encoder.h"
#include "stm32f4xx_hal.h"

using namespace MM;

static constexpr float kWheelDiameterCm = 1.4f;
static constexpr float kGearRatio = 15.0f;

// BSP uses TIM2 encoder mode 3 on both channels, so count all quadrature edges.
static constexpr float kEncoderCountsPerMotorRev = 12.0f;

// Sample time for velocity calculation (100ms)
static constexpr uint32_t kSampleTimeUs = 100'000;

static constexpr float kTicksPerOutputRev =
    kGearRatio * kEncoderCountsPerMotorRev;  // 180 ticks per rev

// Expected: (1.4cm * pi) / 180 ticks = 0.0244 cm/tick
static constexpr float kCmPerTick =
    (kWheelDiameterCm * std::numbers::pi_v<float>) / kTicksPerOutputRev;

/**
* @note This is to replace HAL_GetTick(). CORE cycle counter is used for more accurate timing
*       MCU currently using HSI 16MHz clock, so 16 cycles per microsecond. Adjust if clock changes.
*       So we sampling at frequency of 10Hz, which should be sufficient for testing encoder performance at low speeds.
*       
*/
static void init_cycle_counter()
{
    // Enable DWT cycle counter
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Reset cycle counter
    DWT->CYCCNT = 0;

    // Enable cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
}

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    init_cycle_counter();

    // Calculate cycles per microsecond for timing calculations
    // Expected: 16MHz / 1,000,000 = 16 cycles/us
    const uint32_t cycles_per_us = SystemCoreClock / 1'000'000u;

    // Calculate total cycles to wait for the sample time
    // Expected: 100ms * 16 cycles/us = 1,600,000 cycles
    const uint32_t sample_cycles = kSampleTimeUs * cycles_per_us;

    // Lambda to get current cycle count
    // This fix rounding errors when calculating elapsed time in microseconds
    auto get_time_cycles = []() -> uint32_t
    {
        return DWT->CYCCNT;
    };

    while (1)
    {
        int32_t start_ticks = hw.encoder.get_ticks();
        uint32_t start_time = get_time_cycles();

        while ((get_time_cycles() - start_time) < sample_cycles)
        {
        }

        uint32_t end_time = get_time_cycles();
        uint32_t delta_cycles = end_time - start_time;
        if (delta_cycles == 0 || cycles_per_us == 0)
        {
            continue;
        }

        int32_t end_ticks = hw.encoder.get_ticks();
        int32_t delta_ticks = end_ticks - start_ticks;

        float delta_time_us = static_cast<float>(delta_cycles) /
                              static_cast<float>(cycles_per_us);

        // Convert us to seconds for velocity
        float delta_time_sec = delta_time_us / 1'000'000.0f;

        // Calculate velocity and distance
        float ticks_per_sec = static_cast<float>(delta_ticks) / delta_time_sec;

        // RPM = (60 sec / rev) * (ticks / sec) / (ticks / rev)
        // Expected: (60 / 1) * (ticks/sec) / 180 ticks/rev = ticks/sec * 0.3333 RPM per tick/sec = # RPM
        // IF 6000 Ticks/sec => 6000 * 0.3333 = 2000 RPM, which is reasonable
        float rpm = (60.0f / kTicksPerOutputRev) * ticks_per_sec;

        // Velocity in cm/s = (cm/tick) * (ticks/sec)
        // Expected: 0.0244 cm/tick * ticks/sec = # velocity in cm/s
        float velocity_cm_per_sec = kCmPerTick * ticks_per_sec;

        // Distance in cm = (cm/tick) * (delta ticks)
        // Expected: 0.0244 cm/tick * delta ticks = # distance in cm
        float distance_cm = kCmPerTick * static_cast<float>(delta_ticks);

        (void)rpm;
        (void)ticks_per_sec;
        (void)velocity_cm_per_sec;
        (void)distance_cm;
        (void)delta_time_sec;
        (void)delta_time_us;
        (void)end_ticks;
        (void)delta_ticks;
    }
    return 0;
}
