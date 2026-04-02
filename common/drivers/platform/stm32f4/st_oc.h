/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver for STM32F4
 */
#pragma once

#include <chrono>
#include <cstdint>
#include "oc.h"

struct StOcParams
{
    uint32_t timer_freq;
    uint32_t pclk;
};

namespace MM
{
namespace Stmf4
{
class HwOc : public Oc
{
public:
    explicit HwOc(StOcParams params_);
    bool init();
    bool set_period(std::chrono::microseconds period_us);
    bool set_compare(std::chrono::microseconds compare_us);

private:
    // If changing period in different time unit, have to change all period member variables to match
    std::chrono::microseconds period_us;
};
};  // namespace Stmf4
};  // namespace MM