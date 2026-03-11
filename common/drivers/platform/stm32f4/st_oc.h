/**
 * @file st_oc.h
 * @author Kent Hong
 * @brief Timer Output Compare Driver API for the STM32F411
 */

#pragma once
#include "oc.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{
class HwOc : public Oc
{
public:
    HwOc();
};
};  // namespace Stmf4
};  // namespace MM