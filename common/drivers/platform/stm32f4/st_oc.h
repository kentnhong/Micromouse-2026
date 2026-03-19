/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver API for the STM32F411
 * @note This driver currently only support TIM2 to TIM5
 */

#pragma once
#include "oc.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{

enum class OcDirection : uint8_t
{
    UPCOUNTER = 0,
    DOWNCOUNTER
};


class HwOc : public Oc
{
public:
    HwOc();
};
};  // namespace Stmf4
};  // namespace MM