#include <numbers>
#include "board.h"
#include "delay.h"
#include "encoder.h"
#include "stm32f4xx_hal.h"

using namespace MM;

static constexpr float kWheelDiameterCm = 1.4f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerRev = 12.0f;
static constexpr uint32_t kSampleTimeMs = 100;

static constexpr float kTicksPerOutputRev =
    kGearRatio * kTicksPerRev;  // 180 ticks/output rev
static constexpr float kCmPerTick =
    (kWheelDiameterCm * std::numbers::pi_v<float>) /
    kTicksPerOutputRev;  

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    auto get_time_ms = []() -> uint32_t
    {
        return HAL_GetTick();  // get the current tick count in milliseconds
    };

    while (1)
    {
        int32_t start_ticks = hw.encoder.get_ticks();
        uint32_t start_time = get_time_ms();

        while ((get_time_ms() - start_time) < kSampleTimeMs) {}

        uint32_t end_time = get_time_ms();
        uint32_t delta_time_ms = end_time - start_time;
        if (delta_time_ms == 0)
        {
            continue;
        }

        int32_t end_ticks = hw.encoder.get_ticks();
        int32_t delta_ticks = end_ticks - start_ticks;
        float delta_time_sec = static_cast<float>(delta_time_ms) / 1000.0f;
        float ticks_per_sec = static_cast<float>(delta_ticks) / delta_time_sec;
        float rpm = (60.0f / kTicksPerOutputRev) * ticks_per_sec;
        float velocity_cm_per_sec = kCmPerTick * ticks_per_sec;
        float distance_cm = kCmPerTick * static_cast<float>(delta_ticks);

        (void)rpm;
        (void)ticks_per_sec;
        (void)velocity_cm_per_sec;
        (void)distance_cm;
        (void)delta_time_sec;
        (void)end_ticks;
        (void)delta_ticks;
    }
    return 0;
}
