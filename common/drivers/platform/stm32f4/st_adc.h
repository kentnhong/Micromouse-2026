/**
 * @file st_adc.h
 * @author Kent Hong
 * @brief ADC Driver for the STM32F411
 * 
 */

#pragma once

#include <array>
#include <cstdint>
#include "adc.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{

enum class AdcChannel : uint8_t
{
    NONE = 0,
    CH0,
    CH1,
    CH2,
    CH3,
    CH4,
    CH5,
    CH6,
    CH7,
    CH8,
    CH9,
    CH10,
    CH11,
    CH12,
    CH13,
    CH14,
    CH15
};

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

enum class AdcClkPrescaler : uint8_t
{
    PCLK2_DIV_2 = 0,
    PCLK2_DIV_4,
    PCLK2_DIV_6,
    PCLK2_DIV_8
};

struct StAdcSettings
{
    AdcResolution resolution;
    AdcClkPrescaler prescaler;
};

struct StAdcParams
{
    StAdcSettings settings;
    ADC_TypeDef* base_addr;
    ADC_Common_TypeDef* common_base_addr;
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
     * @brief Continuous conversion of a single adc channel's analog to digital values
     * 
     * @return true Conversion successful, false otherwise
     *
     * Valid channels for the F411 are 0-15
     */
    bool convert(uint8_t channel) override;

    /**
     * @brief Read converted analog values from ADC buffer
     * 
     * @return true Read successful, false otherwise
     */
    bool read() override;

private:
    StAdcSettings settings;
    ADC_TypeDef* base_addr;
<<<<<<< HEAD
    ADC_Common_TypeDef* common_base_addr;
=======
    std::array<AdcChannel, 16> ch_sequence;
    static constexpr uint32_t kSeqMask = 0x1F;  // 0001 1111
>>>>>>> 8871dec (Channel sequence calculation)
};
};  // namespace Stmf4
};  // namespace MM