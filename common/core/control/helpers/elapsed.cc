#include "elapsed.h"

namespace MM
{

static constexpr float MS_TO_SEC = 1000.0f;

float Elapsed::get_dt_sec()
{
    static uint32_t prev = Utils::get_ms_ticks();
    uint32_t now = Utils::get_ms_ticks();

    // Convert ms to seconds
    float dt_sec = (now - prev) / MS_TO_SEC;

    prev = now;

    return dt_sec;
};

}  // namespace MM