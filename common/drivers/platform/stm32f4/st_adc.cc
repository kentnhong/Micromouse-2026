#include "st_adc.h"

namespace MM
{
namespace Stmf4
{
HwAdc::HwAdc(StAdcParams& params_)
    : settings{params_.settings},
      base_addr{params_.base_addr},
      common_base_addr{params_.common_base_addr}
{
}

bool HwAdc::init()
{
    // Configure PCLK2 prescaler in ADC_CCR
    common_base_addr->CCR &= ~ADC_CCR_ADCPRE;
    switch (settings.prescaler)
    {
        case AdcClkPrescaler::PCLK2_DIV_2:
            break;
        case AdcClkPrescaler::PCLK2_DIV_4:
            common_base_addr->CCR |= ADC_CCR_ADCPRE_0;
            break;
        case AdcClkPrescaler::PCLK2_DIV_6:
            common_base_addr->CCR |= ADC_CCR_ADCPRE_1;
            break;
        case AdcClkPrescaler::PCLK2_DIV_8:
            common_base_addr->CCR |= ADC_CCR_ADCPRE;
    }

    // Set resolution in ADC_CR1
    base_addr->CR1 &= ~ADC_CR1_RES;
    switch (settings.resolution)
    {
        case AdcResolution::TWELVE_BIT:
            break;
        case AdcResolution::TEN_BIT:
            base_addr->CR1 |= ADC_CR1_RES_0;
            break;
        case AdcResolution::EIGHT_BIT:
            base_addr->CR1 |= ADC_CR1_RES_1;
            break;
        case AdcResolution::SIX_BIT:
            base_addr->CR1 |= ADC_CR1_RES;
    }

    // Configure right align in ADC_CR2
    base_addr->CR2 &= ~ADC_CR2_ALIGN;

    // Turn on ADC in ADC_CR2
    base_addr->CR2 |= ADC_CR2_ADON;

    return true;
}

bool HwAdc::convert(uint8_t channel)
{
    // Check if channel is within valid range of F411 ADC channels
    if (channel < 0 || channel > 15)
    {
        return false;
    }

    // Check if a conversion is in progress
    while (base_addr->SR & ADC_SR_STRT);

    // Set channel to be converted in SQ1 in ADC_SQR3

    // Set continuous conversion mode in ADC_CR2

    // Check for overrun
    if (base_addr->SR & ADC_SR_OVR)
    {
        // Reinitialize the DMA (adjust destination address and NDTR counter)
        // Clear the ADC OVR bit in ADC_SR register
        // Trigger the ADC to start the conversion
    }

    // Enable DMA requests
    base_addr->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;

    // Set start conversion of regular channels in ADC_CR2

    /*
    After each conversion:
        • If a regular group of channels was converted:
            – The last converted data are stored into the 16-bit ADC_DR register
            – The EOC (end of conversion) flag is set (cleared by reading the ADC_DR register)
            – An interrupt is generated if the EOCIE bit is set
    */

    // Disable continuous conversion

    // Stop DMA requests

    return true;
}
};  // namespace Stmf4
};  // namespace MM