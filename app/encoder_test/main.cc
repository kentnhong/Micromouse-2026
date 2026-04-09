#include <numbers>
#include "board.h"
#include "encoder.h"

using namespace MM;

static constexpr float kWheelDiameterCm =
    2.794f;  // [cm] 1.1 inch (2.794cm) diameter wheel
static constexpr float kWheelCircumferenceCm =
    kWheelDiameterCm *
    std::numbers::pi_v<float>;  // [cm] Circumference = diameter * pi
static constexpr int kTicksPerRevolution =
    12 * 15;  // 12 ticks * 15:1 gear ratio (180)
static constexpr float kDistancePerTickCm =
    kWheelCircumferenceCm / kTicksPerRevolution;
static constexpr float kSampleIntervalSec =
    1.0f;  // 1 second sample window meaning 1Hz velocity update rate
static constexpr int kSampleCount = 100;  // 100Hz sampling

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    int32_t prev_ticks = hw.encoder.get_ticks();
    int32_t tick_acc = 0;

    while (1)
    {
        int32_t current_ticks = prev_ticks;

        // Loop through about 100 samples in 1 second
        for (int i = 0; i < kSampleCount; ++i)
        {
            current_ticks = hw.encoder.get_ticks();
        }

        // Calculate velocity, distance, and RPM based on tick counts
        int32_t delta_ticks = current_ticks - prev_ticks;
        prev_ticks = current_ticks;

        // Calculate velocity in cm/s
        float velocity_cmps =
            (delta_ticks * kDistancePerTickCm) / kSampleIntervalSec;

        // Distance is total ticks * distance per tick (cm)
        float distance_cm = current_ticks * kDistancePerTickCm;

        // Formula for RPM: (delta_ticks / ticks_per_revolution) * (60 seconds / sample_interval_seconds)
        float rpm = (delta_ticks / static_cast<float>(kTicksPerRevolution)) *
                    (60.0f / kSampleIntervalSec);

        // Accumulate ticks to determine when a full revolution is completed
        tick_acc += delta_ticks;
        bool revolution_complete = false;

        // Check if we've completed a full revolution
        if (tick_acc >= kTicksPerRevolution)
        {
            tick_acc -= kTicksPerRevolution;
            revolution_complete = true;
        }

        (void)velocity_cmps;
        (void)distance_cm;
        (void)rpm;
        (void)revolution_complete;
    }

    return 0;
}