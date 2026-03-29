#include "pid.h"

namespace MM
{

static constexpr kMinOutput = -100.0f;  // Replace with actual value later
static constexpr kMaxOutput = 100.0f;   // Same as above

PID::PID(Bno055Data& imu_data, EncoderInput& enc_data, Val& pid)
{
}

bool PID::update(const Input& input, const Target& target, Output& output)
{
}

bool PID::reset()
{
}

bool PID::set_output_limits(float min_output, float max_output)
{
    min_output = kMinOutput;
    max_output = kMaxOutput;
}

}  // namespace MM