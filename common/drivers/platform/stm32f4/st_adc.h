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

enum class AdcResolution : uint8_t
{
    TWELVE_BIT = 0,
    TEN_BIT,
    EIGHT_BIT,
    SIX_BIT
};

enum class AdcDma : uint8_t
{
    DMA_DISABLE = 0,
    DMA_ENABLE
};

struct StAdcSettings
{
    AdcResolution resolution;
    AdcDma dma;
};
struct StAdcParams
{
    StAdcSettings settings;
    ADC_TypeDef* base_addr;
};
class HwAdc : public Adc
{
public:
    explicit HwAdc(StAdcParams& params_);

    /**
     * @brief Initialize ADC peripheral
     * 
     * @return true ADC initialized successfully, false otherwise
     */
    bool init();

    /**
     * @brief Continuous conversion of analog to digital values
     * 
     * @return true Conversion successful, false otherwise
     */
    bool convert() override;

    /**
     * @brief Read converted analog values from ADC buffer
     * 
     * @return true Read successful, false otherwise
     */
    bool read() override;

private:
    StAdcSettings settings;
    ADC_TypeDef* base_addr;
};
};  // namespace Stmf4
};  // namespace MM