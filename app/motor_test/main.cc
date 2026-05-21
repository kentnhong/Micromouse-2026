#include "board.h"

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

int main()
{
    bsp_init();
    Board& hw = get_board();

    // pwm
    hw.pwm1.set_frequency(2000);
    hw.pwm2.set_frequency(2000);

    // encoder
    uint32_t cycles_per_us = hw.encoder.cycles_per_us();
    const uint32_t sample_cycles = kSampleTimeUs * cycles_per_us;

    // Drv8231
    hw.motor.drive(Drv8231::Direction::FORWARD, 50);

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
}