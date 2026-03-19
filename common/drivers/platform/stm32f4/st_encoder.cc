#include "st_encoder.h"
#include <cstdint>
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4
{

static constexpr uint8_t kTimCcmrxCcsBitWidth = 2;
static constexpr uint8_t kTimSmcrSmsBitWidth = 3;

HwEncoder::HwEncoder(const StEncoderParams& params)
    : base_addr{params.base_addr},
      settings{params.settings},
      current_ticks{0},
      prev_ticks{0}
{
}
// TIM 1..5 support encoder mode with CH1 and CH2 only - return false if other channels used
static inline bool is_timer_1_to_5(TIM_TypeDef* t)
{
    return (t == TIM1 || t == TIM2 || t == TIM3 || t == TIM4 || t == TIM5);
}
static inline bool is_timer_9_to_11(TIM_TypeDef* t)
{
    return (t == TIM9 || t == TIM10 || t == TIM11);
}

bool HwEncoder::init()
{
    if (base_addr == nullptr)
    {
        return false;
    }

    // Timer & Channel validatity
    if (is_timer_1_to_5(base_addr))
    {

        // Encoder mode with CH1 and CH2 or BOTH only - return false if other channels used
        if (channel != EncChannel::CH1 && channel != EncChannel::CH2 &&
            channel != EncChannel::BOTH)
        {
            return false;
        }
    }
    else if (is_timer_9_to_11(base_addr))
    {
        return false;
    }

    // Configure both CH1 and CH2 for encoder mode
    if (channel == EncChannel::BOTH)
    {
        // Clear CC1S and CC2S bits to select the input for both channels
        base_addr->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_CC2S_Msk);

        // Set CC1S and CC2S
        SetReg(&base_addr->CCMR1, static_cast<uint32_t>(settings.mode),
               TIM_CCMR1_CC1S_Pos, kTimCcmrxCcsBitWidth);
        SetReg(&base_addr->CCMR1, static_cast<uint32_t>(settings.mode),
               TIM_CCMR1_CC2S_Pos, kTimCcmrxCcsBitWidth);

        // Clear polarity bits for both channels
        base_addr->CCER &=
            ~(TIM_CCER_CC1P | TIM_CCER_CC1NP | TIM_CCER_CC2P | TIM_CCER_CC2NP);

        uint32_t polarity_bits = 0;
        switch (settings.polarity)
        {
            case EncInputPolarity::RISING:
                break;
            case EncInputPolarity::FALLING:
                polarity_bits = TIM_CCER_CC1P | TIM_CCER_CC2P;
                break;
            case EncInputPolarity::BOTH:
                polarity_bits = TIM_CCER_CC1P | TIM_CCER_CC1NP | TIM_CCER_CC2P |
                                TIM_CCER_CC2NP;
                break;
        }
        base_addr->CCER |= polarity_bits;

        // Set encoder mode (SMS bits)
        SetReg(&base_addr->SMCR, static_cast<uint32_t>(settings.mode),
               TIM_SMCR_SMS_Pos, kTimSmcrSmsBitWidth);

        // Enable timer counter
        base_addr->CR1 |= TIM_CR1_CEN;
        return true;
    }

    // Configure the specified channel for encoder mode
    switch (channel)
    {
        case EncChannel::CH1:
            // Clear the CC1S bits to select the input for CH1
            base_addr->CCMR1 &= ~TIM_CCMR1_CC1S_Msk;

            // Set the mode for CH1
            SetReg(&base_addr->CCMR1, static_cast<uint32_t>(settings.mode),
                   TIM_CCMR1_CC1S_Pos, kTimCcmrxCcsBitWidth);

            // Set the polarity for CH1
            SetReg(&base_addr->CCER, static_cast<uint32_t>(settings.polarity),
                   TIM_CCER_CC1P_Pos, 0);

            // Set the slave mode for CH1
            SetReg(&base_addr->SMCR, static_cast<uint32_t>(settings.slave_mode),
                   TIM_SMCR_SMS_Pos, kTimSmcrSmsBitWidth);

            break;
        case EncChannel::CH2:
            // Clear the CC2S bits to select the input for CH2
            base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;

            // Set the mode for CH2
            SetReg(&base_addr->CCMR1, static_cast<uint32_t>(settings.mode),
                   TIM_CCMR1_CC2S_Pos, kTimCcmrxCcsBitWidth);

            // Set the polarity for CH2
            SetReg(&base_addr->CCER, static_cast<uint32_t>(settings.polarity),
                   TIM_CCER_CC2P_Pos, 0);

            // Set the slave mode for CH2
            SetReg(&base_addr->SMCR, static_cast<uint32_t>(settings.slave_mode),
                   TIM_SMCR_SMS_Pos, kTimSmcrSmsBitWidth);
            break;
        default:
            return false;
    }

    // Enable the time counter
    base_addr->CR1 |= TIM_CR1_CEN;

    return true;
}

int32_t HwEncoder::get_ticks()
{
    // Read the current encoder ticks from the timer's CNT register
    current_ticks = static_cast<int32_t>(base_addr->CNT);
    return current_ticks;
}

bool HwEncoder::reset_ticks()
{
    // Reset the encoder ticks to zero
    base_addr->CNT = 0;
    current_ticks = 0;
    prev_ticks = 0;
    return true;
}

}  // namespace Stmf4
}  // namespace MM
