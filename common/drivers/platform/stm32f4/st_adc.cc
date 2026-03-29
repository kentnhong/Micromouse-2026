#include "st_adc.h"

namespace
{
// Each SQx field in SQR registers is 5 bits wide (stores channel index 0..18).
constexpr uint32_t kSqrBitsMask{0x1F};
// F411 regular external channels we support.
constexpr uint8_t kMinCh{0};
constexpr uint8_t kMaxCh{15};
// Regular conversion rank range on STM32F4 is 1..16.
constexpr uint8_t kMinRank{1};
constexpr uint8_t kMaxRank{16};
// SQRx registers hold 6 ranks each (SQR3: 1-6, SQR2: 7-12, SQR1: 13-16).
constexpr uint8_t kRanksPerReg{6};
// Bit width of one rank field in SQRx.
constexpr uint8_t kSqrBitsPerRank{5};
// Channel 10+ sample-time fields are in SMPR1; 0..9 are in SMPR2.
constexpr uint8_t kSmpr1Begin{10};
// Bit width of one sample-time field in SMPRx.
constexpr uint8_t kSmprBitsPerCh{3};
// 3-bit mask for a sample-time field.
constexpr uint32_t kSmprMask{0x7};

constexpr bool is_valid_ch(uint8_t ch)
{
    // Allow only channels this driver currently supports.
    return ch >= kMinCh && ch <= kMaxCh;
}

constexpr bool is_valid_rank(uint8_t rank)
{
    // Regular sequence ranks are 1-based and limited to 16 entries.
    return rank >= kMinRank && rank <= kMaxRank;
}
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

    // Enable DMA req if chosen
    en_dma_req();

    // Enable overrun interrupt if chosen
    if (settings.overrun_int == AdcOverrunInt::OVRIE_EN)
    {
        base_addr->CR1 |= ADC_CR1_OVRIE;
    }

    if (!settings.sequence.empty())
    {
        // Hardware supports up to 16 regular conversion ranks.
        if (settings.sequence.size() > kMaxRank)
            return false;

        // Start from a known clean sequence mapping.
        base_addr->SQR3 &= ~(ADC_SQR3_SQ6 | ADC_SQR3_SQ5 | ADC_SQR3_SQ4 |
                             ADC_SQR3_SQ3 | ADC_SQR3_SQ2 | ADC_SQR3_SQ1);
        base_addr->SQR2 &= ~(ADC_SQR2_SQ12 | ADC_SQR2_SQ11 | ADC_SQR2_SQ10 |
                             ADC_SQR2_SQ9 | ADC_SQR2_SQ8 | ADC_SQR2_SQ7);
        base_addr->SQR1 &= ~(ADC_SQR1_L | ADC_SQR1_SQ13 | ADC_SQR1_SQ14 |
                             ADC_SQR1_SQ15 | ADC_SQR1_SQ16);

        // L stores (number of conversions - 1).
        base_addr->SQR1 |= (static_cast<uint32_t>(settings.sequence.size() - 1)
                            << ADC_SQR1_L_Pos);

        // Program ranks in order: sequence[0] -> rank1, sequence[1] -> rank2, ...
        for (size_t i = 0; i < settings.sequence.size(); i++)
        {
            if (!set_channel(static_cast<uint8_t>(i + 1), settings.sequence[i]))
                return false;
        }
    }

    // Apply per-channel sample-time settings after sequence is set.
    for (const auto& cfg : settings.ch_cycles)
    {
        if (!set_cycles(cfg.ch, cfg.cycles))
            return false;
    }

    return true;
}

bool HwAdc::convert(bool single, size_t samples)
{
    if (settings.source == AdcTriggerSource::EXTERNAL)
        return false;

    if (!single)
    {
        // Set continuous conversion mode in ADC_CR2
        base_addr->CR2 |= ADC_CR2_CONT;
        // Check if a conversion is in progress
        while (base_addr->SR & ADC_SR_STRT);
        // Set start conversion of regular channel in ADC_CR2
        base_addr->CR2 |= ADC_CR2_SWSTART;
    }
    else
    {
        // Disable continuous conversion mode in ADC_CR2
        base_addr->CR2 &= ~ADC_CR2_CONT;
        for (size_t i = 0; i < samples; i++)
        {
            // Check if a conversion is in progress
            while (base_addr->SR & ADC_SR_STRT);
            // Set start conversion of regular channel in ADC_CR2
            base_addr->CR2 |= ADC_CR2_SWSTART;
        }
    }
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

bool HwAdc::read(uint32_t& val)
{
    // Wait until ADC conversion is finished
    while ((base_addr->SR & ADC_SR_EOC) == 0);

    // Read from ADC DR
    val = static_cast<uint16_t>(base_addr->DR);

    return true;
}

bool HwAdc::set_channel(uint8_t rank, uint8_t ch)
{
    // Reject invalid rank/channel before touching registers.
    if (!is_valid_rank(rank) || !is_valid_ch(ch))
        return false;

    // Convert 1-based rank to 0-based index.
    const uint8_t idx = static_cast<uint8_t>(rank - 1);
    // Position of this rank field inside SQRx register.
    const uint8_t shift =
        static_cast<uint8_t>((idx % kRanksPerReg) * kSqrBitsPerRank);
    // Mask for only this rank field.
    const uint32_t mask = (kSqrBitsMask << shift);

    // Pick target SQR register from rank range.
    volatile uint32_t* sqr = &base_addr->SQR3;
    if (rank > 12)
        sqr = &base_addr->SQR1;
    else if (rank > 6)
        sqr = &base_addr->SQR2;

    // Clear previous channel at this rank and write the new channel id.
    *sqr = (*sqr & ~mask) | (static_cast<uint32_t>(ch) << shift);

    return true;
}

bool HwAdc::set_cycles(uint8_t ch, AdcCycles cycles)
{
    // Check if ch is in valid range
    if (!is_valid_ch(ch))
        return false;

    // Map channel to SMPR field index (0..9 in SMPR2, 10..15 in SMPR1).
    uint8_t pos = ch < kSmpr1Begin ? ch : (ch - kSmpr1Begin);
    // Bit offset of that channel's 3-bit sample-time field.
    const uint8_t shift = static_cast<uint8_t>(pos * kSmprBitsPerCh);
    // Mask for only this channel field.
    const uint32_t mask = (kSmprMask << shift);

    if (ch < kSmpr1Begin)
    {
        // Update field in SMPR2 for channels 0..9.
        base_addr->SMPR2 &= ~mask;
        base_addr->SMPR2 |= (static_cast<uint32_t>(cycles) << shift);
    }
    else
    {
        // Update field in SMPR1 for channels 10..15.
        base_addr->SMPR1 &= ~mask;
        base_addr->SMPR1 |= (static_cast<uint32_t>(cycles) << shift);
    }

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

bool HwAdc::ovr_recover()
{
    // Check if overrun occurred
    if (!(base_addr->SR & ADC_SR_OVR))
        return false;

    // Clear OVR bit
    base_addr->SR &= ~ADC_SR_OVR;

    return true;
}
};  // namespace Stmf4
};  // namespace MM
