#include <numbers>
#include "board.h"
#include "delay.h"
#include "encoder.h"

using namespace MM;

static constexpr float kWheelDiameterCm = 1.4f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kEncoderCountsPerMotorRev = 12.0f;
static constexpr uint32_t kSampleTimeUs =
    100'000;  // Sample time for velocity calculation (100ms)

static constexpr float kTicksPerOutputRev =
    kGearRatio * kEncoderCountsPerMotorRev;  // # ticks per output rev

static constexpr float kCmPerTick =
    (kWheelDiameterCm * std::numbers::pi_v<float>) / kTicksPerOutputRev;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    hw.encoder.init_cycle_counter();

    // Expected: 16MHz / 1,000,000 = 16 cycles/us
    uint32_t cycles_per_us = hw.encoder.cycles_per_us();

    // Expected: 100ms * 16 cycles/us = 1,600,000 cycles
    const uint32_t sample_cycles = kSampleTimeUs * cycles_per_us;

    while (1)
    {
        EncoderStats stats = hw.encoder.measure_encoder_stats(
            sample_cycles, kTicksPerOutputRev, kCmPerTick);
        volatile float rpm = stats.rpm;
        volatile float velocity_cm_per_sec = stats.velocity_cm_per_sec;
        volatile float distance_cm = stats.distance_cm;
        volatile float delta_time_sec = stats.delta_time_sec;
        volatile int32_t delta_ticks = stats.delta_ticks;

        (void)rpm;
        (void)velocity_cm_per_sec;
        (void)distance_cm;
        (void)delta_time_sec;
        (void)delta_ticks;
    }
    return 0;
}
