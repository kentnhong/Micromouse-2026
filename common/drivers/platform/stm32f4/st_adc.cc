#include "st_adc.h"

namespace
{
constexpr uint32_t kSq1Mask{0xFFFFFFE0};
constexpr uint8_t kMinCh{0};
constexpr uint8_t kMaxCh{15};
constexpr uint8_t kSmpr1Begin{10};
constexpr uint8_t kSmprBitsPerCh{3};
};  // namespace
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

    // Clear overrun interrupt bit
    base_addr->CR1 &= ~ADC_CR1_OVRIE;

    // Enable overrun interrupt if chosen
    if (settings.overrun_int == AdcOverrunInt::OVRIE_EN)
    {
        base_addr->CR1 |= ADC_CR1_OVRIE;
    }

    return true;
}

bool HwAdc::convert(bool single)
{
    if (settings.source == AdcTriggerSource::EXTERNAL)
        return false;
    
    // Check if a conversion is in progress
    while (base_addr->SR & ADC_SR_STRT);

    // Disable continuous conversion mode in ADC_CR2
    base_addr->CR2 &= ~ADC_CR2_CONT;

    if (!single)
    {
        // Set continuous conversion mode in ADC_CR2
        base_addr->CR2 |= ADC_CR2_CONT;
    }

    // Set start conversion of regular channel in ADC_CR2
    base_addr->CR2 |= ADC_CR2_SWSTART;

    return true;
}

bool HwAdc::en_dma_req()
{
    if (settings.dma == AdcDma::DMA_ENABLE)
    {
        // Enable DMA requests
        base_addr->CR2 |= ADC_CR2_DMA | ADC_CR2_DDS;
        return true;
    }
    return false;
}

bool HwAdc::read(uint16_t& val)
{
    // Wait until ADC conversion is finished
    while ((base_addr->SR & ADC_SR_EOC) == 0);

    // Read from ADC DR
    val = static_cast<uint16_t>(base_addr->DR);

    return true;
}

bool HwAdc::set_channel(uint8_t ch)
{
    // Check if channel is within valid range of F411 ADC channels
    if (ch < kMinCh || ch > kMaxCh)
        return false;

    // Set channel to be converted in SQ1 in ADC_SQR3 (Only one channel conversion at a time for now)
    base_addr->SQR3 = (base_addr->SQR3 & kSq1Mask) |
                      (static_cast<uint32_t>(ch) << ADC_SQR3_SQ1_Pos);

    return true;
}

bool HwAdc::set_cycles(uint8_t ch, AdcSampleTime cycles)
{
    // Check if ch is in valid range
    if (ch < kMinCh || ch > kMaxCh)
        return false;

    uint8_t pos = ch < kSmpr1Begin ? ch : (ch - kSmpr1Begin);

    if (ch < kSmpr1Begin)
        base_addr->SMPR2 |=
            (static_cast<uint32_t>(cycles) << (pos * kSmprBitsPerCh));
    else
        base_addr->SMPR1 |=
            (static_cast<uint32_t>(cycles) << (pos * kSmprBitsPerCh));

    return true;
}

bool HwAdc::set_ext_trigger(HwAdc::ExternalEvent event,
                            HwAdc::TriggerPolarity polarity)
{
    if (settings.source == AdcTriggerSource::SOFTWARE)
        return false;

    base_addr->CR2 &= ~(ADC_CR2_EXTSEL | ADC_CR2_EXTEN);
    base_addr->CR2 |= static_cast<uint32_t>(event) << ADC_CR2_EXTSEL_Pos;
    base_addr->CR2 |= static_cast<uint32_t>(polarity) << ADC_CR2_EXTEN_Pos;

    return true;
}

bool HwAdc::ovr_recover(bool dma_reinit)
{
    // Check if overrun occurred
    if (!(base_addr->SR & ADC_SR_OVR))
        return false;

    // Clear OVR bit
    base_addr->SR &= ~ADC_SR_OVR;

    // Check if DMA was reinitialized and trigger ADC conversion
    if (dma_reinit)
        convert(false);  // Continuous conversion for DMA mode

    return true;
}
};  // namespace Stmf4
};  // namespace MM