/**
 * @file st_adc.h
 * @author Kent Hong
 * @brief ADC Driver for the STM32F411
 * 
 */

#pragma once

#include <span>

#include "adc.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{

enum class AdcTriggerSource : uint8_t
{
    SOFTWARE = 0,
    EXTERNAL
};

enum class AdcOverrunInt : uint8_t
{
    OVRIE_DIS = 0,
    OVRIE_EN
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

enum class AdcCycles : uint8_t
{
    CYCLES_3 = 0,
    CYCLES_15,
    CYCLES_28,
    CYCLES_56,
    CYCLES_84,
    CYCLES_112,
    CYCLES_144,
    CYCLES_480
};

struct AdcChCycles
{
    uint8_t ch;
    AdcCycles cycles;
};

struct StAdcSettings
{
    AdcResolution resolution;
    AdcClkPrescaler prescaler;
    AdcTriggerSource source;
    AdcOverrunInt overrun_int;
    AdcDma dma;
    std::span<const uint8_t>
        sequence{};  // The order in which ADC channels are converted in the sequence
    std::span<const AdcChCycles> ch_cycles{};
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

    enum class TriggerPolarity : uint8_t
    {
        DISABLED = 0,
        RISING_EDGE,
        FALLING_EDGE,
        BOTH_EDGES
    };

    enum class ExternalEvent : uint8_t
    {
        TIM1_CH1 = 0,
        TIM1_CH2,
        TIM1_CH3,
        TIM2_CH2,
        TIM2_CH3,
        TIM2_CH4,
        TIM2_TRGO,
        TIM3_CH1,
        TIM3_TRGO,
        TIM4_CH4,
        TIM5_CH1,
        TIM5_CH2,
        TIM5_CH3,
        EXTI_LINE = 15
    };

    /**
     * @brief Initialize ADC peripheral
     * 
     * @return true ADC initialized successfully, false otherwise
     */
    bool init();

    /**
    * @brief Single or continuous conversion of an ADC channel
    * 
    * @param single True for single. False for continuous.
    * @param samples How many ADC samples for single shot conversions (obsolete if single is set to false)
    * @return true Conversion successful, false otherwise
    */
    bool convert(bool single, size_t samples) override;

    /**
     * @brief Read converted analog values from ADC buffer (for non-DMA)
     * 
     * @return true Read successful, false otherwise
     */
    bool read(uint32_t& val) override;

    /**
     * @brief Enable DMA for ADC (might have to enable DMA again if overrun, that's why its not in init)
     * 
     * @return true success, false otherwise
     */
    bool en_dma_req();

    /**
     * @brief Set external ADC trigger event and trigger polarity
     * 
     * @param trigger The specific hardware event to trigger the ADC
     * @param polarity What type of edge detection for ADC triggers
     * @return true success, false otherwise
     */
    bool set_ext_trigger(ExternalEvent event, TriggerPolarity polarity);

    /**
     * @brief Recovers ADC from OVR state
     * 
     * @return true ADC recovered, false otherwise
     */
    bool ovr_recover();

    /**
     * @brief Set ADC channel at a specific sequence position
     *
     * @param rank Order of ADC channel to be converted for a sequence of conversions
     * @param ch ADC channel
     */
    bool set_channel(uint8_t rank, uint8_t ch) override;

private:
    /**
     * @brief Set the sample time (cycles) for the specified ADC channel
     * 
     * @param ch ADC Channel
     * @param cycles Sample Time in Cycles
     * @return true success, false otherwise
     * @note We dont support CH 16-18 at this time for the temperature sensor
     */
    bool set_cycles(uint8_t ch, AdcCycles cycles);

    StAdcSettings settings;
    ADC_TypeDef* base_addr;
    ADC_Common_TypeDef* common_base_addr;
};
};  // namespace Stmf4
};  // namespace MM