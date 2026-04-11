#include <numbers>
#include "board.h"
#include "delay.h"
#include "encoder.h"
#include "stm32f4xx_hal.h"

using namespace MM;

static constexpr float kWheelDiameterCm = 2.79f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerRev = 12.0f;

static constexpr float kTicksPerOutputRev =
    kGearRatio * kTicksPerRev;  // 180 ticks/rev

static constexpr float kCmPerTick =
    (kWheelDiameterCm * std::numbers::pi_v<float>) /
    kTicksPerOutputRev;  // 0.0155 cm/tick

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    /**
    * @note The implementation of the encoder -> RPM -> speed conversion in cm per second 
    *       steps:
    *       Given: 15:1 gear ratio, 12 ticks/rev, wheel diameter to circumference (1.1 inches -> 2.79 cm)
    *              60 sec = 1 min | 1 rev = 12 ticks 
    *               1. Calculate the number of ticks per rev of the output shaft: 15 (gear ratio) * 12 (ticks/rev) = 180 ticks/rev
    *               2. That means 2 degree per tick (360 degrees / 180 ticks)
    *               3. Calculate the distance traveled per tick: (2.79 cm/rev) / 180 ticks/rev = 0.0155 cm/tick
    *               4. To get more accurate measurements, I find the change of ticks over the change in time so t4 - t1 / 4 (number of measurements) 
    *                  to get the average time per tick, then take the inverse to get ticks per second, and multiply by 0.0155 cm/tick to get cm/s
    *               5. Convert this value to mm per second by multiplying by 10 (1 cm = 10 mm) and 2 deg -> 0.0155 cm/tick * 10 mm/cm = 0.155 mm/1 deg x 2 deg/tick = 0.3 mm/tick
    *               6. To convert to RPM, take the ticks per second and divide by 180 (ticks/rev) and multiply by 60 (sec/min) to get RPM
    */

    auto get_time_ms = []() -> uint32_t
    {
        return HAL_GetTick();  // get the current tick count in milliseconds
    };

    while (1)
    {
        // Wait for the first tick
        uint32_t start_ticks = hw.encoder.get_ticks();
        uint32_t start_time = get_time_ms();

        // Wait until 4 more ticks have occurred
        int32_t target_ticks = start_ticks + 4;
        while (hw.encoder.get_ticks() < target_ticks)
        {
            Utils::DelayMs(1);  // in case busy wait
        }

        uint32_t end_time = get_time_ms();

        float delta_time_sec = (end_time - start_time) / 1000.0f;  // ms to sec
        float delta_ticks_per_second = 4.0f / delta_time_sec;

        // Original: (1 rev / 180 ticks) * (60 sec/ 1 min) * (4 ticks / delta_ticks per second) = RPM
        // Simplified: (60 sec / 180 ticks) * delta_ticks_per_second = RPM
        float rpm = (60.0f / kTicksPerOutputRev) * delta_ticks_per_second;

        // Find the speed in cm/s: (0.0155 cm/tick) * delta_ticks_per_second = cm/s
        float speed_cm_per_sec = kCmPerTick * delta_ticks_per_second;

        // Find distance traveled in 4 ticks: (0.0155 cm/tick) * 4 ticks = cm
        float distance_cm = kCmPerTick * 4.0f; 

        (void)rpm;
        (void)delta_ticks_per_second;
        (void)delta_time_sec;
        (void)end_time;
        (void)start_time;
        (void)start_ticks;
        (void)target_ticks;
        (void)speed_cm_per_sec;
        (void)distance_cm;
    }

    return 0;
}