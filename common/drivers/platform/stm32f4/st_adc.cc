#include "st_adc.h"

namespace MM
{
namespace Stmf4
{
HwAdc::HwAdc(StAdcParams& params_)
    : settings{params_.settings},
      base_addr{params_.base_addr},
      ch_sequence{params_.ch_sequence}
{
}

bool HwAdc::init()
{
    // Configure ADC sequence registers from channel sequence array
    uint8_t idx = 0;
    while (idx < 16 && (ch_sequence[idx] != AdcChannel::NONE))
    {
        // Convert channel from enum to bit mapped value
        uint32_t channel = static_cast<uint32_t>(ch_sequence[idx]) - 1;
        if (idx < 6)
        {
            // Configure ADC_SQR3 SQ1 - SQ6
            base_addr->SQR3 &= ~(kSeqMask << (idx * kNumSeqBits));
            base_addr->SQR3 |= (channel << (idx * 5));
        }
        else if (idx > 5 && idx < 12)
        {
            // Configure ADC_SQR2 SQ7 - SQ12
            base_addr->SQR2 &= ~(kSeqMask << ((idx - 6) * kNumSeqBits));
            base_addr->SQR2 |= (channel << ((idx - 6) * kNumSeqBits));
        }
        else
        {
            // Configure ADC_SQR1 SQ13 - SQ16
            base_addr->SQR1 &= ~(kSeqMask << ((idx - 12) * kNumSeqBits));
            base_addr->SQR1 |= (channel << ((idx - 12) * kNumSeqBits));
        }
        idx++;
    }

    // Edge Case no channels in sequence
    if (idx == 0)
    {
        return false;
    }

    // Figure out sequence length in bit mapped value and set the length in ADC_SQR1
    uint8_t seq_length = idx - 1;
    base_addr->SQR1 &= ~(ADC_SQR1_L_Msk);
    base_addr->SQR1 |= (seq_length << ADC_SQR1_L_Pos) & ADC_SQR1_L_Msk;

    // Turn on ADC
    // Start conversion
    return true;
}
};  // namespace Stmf4
};  // namespace MM