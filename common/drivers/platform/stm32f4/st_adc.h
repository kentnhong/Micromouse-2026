/**
 * @file st_adc.h
 * @author Kent Hong
 * @brief ADC Driver for the STM32F411
 * 
 */

#pragma once

#include "adc.h"
#include "stm32f411xe.h"


namespace MM
{
namespace Stmf4
{
struct StAdcParams
{
    ADC_TypeDef* base_addr;
};
class HwAdc
{
public:
    explicit HwAdc(StAdcParams& params);
    bool convert();
    bool read();
};
};  // namespace Stmf4
};  // namespace MM