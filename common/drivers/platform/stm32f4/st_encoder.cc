#include "st_encoder.h"
#include <cstdint>
#include "reg_helpers.h"

namespace MM
{
namespace Stmf4
{

static constexpr uint8_t kTimCcmrxCcsBitWidth = 2;
static constexpr uint8_t kTimSmcrSmsBitWidth = 3;
static constexpr uint32_t kInputMappedOnTi = 0x1u;
static constexpr uint32_t kMaxAutoReload = 0xFFFFFFFFu;

HwEncoder::HwEncoder(const StEncoderParams& params)
    : base_addr{params.base_addr},
      channel{params.settings.channel},
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

static inline uint32_t EncoderModeToSms(EncMode mode)
{
    switch (mode)
    {
        case EncMode::MODE_1:
            return static_cast<uint32_t>(mode);
        case EncMode::MODE_2:
            return static_cast<uint32_t>(mode);
        case EncMode::MODE_3:
            return static_cast<uint32_t>(mode);
        default:
            return 0;
    }
}

static inline uint32_t EncoderPolarityToCcer(EncInputPolarity polarity,
                                             EncChannel channel)
{
    switch (polarity)
    {
        case EncInputPolarity::RISING:
            if (channel == EncChannel::CH1)
            {
                return 0u;  // Rising edge is default, so no bits need to be set
            }
            if (channel == EncChannel::CH2)
            {
                return 0u;  // Rising edge is default, so no bits need to be set
            }
        case EncInputPolarity::BOTH:
            return 0u;
        case EncInputPolarity::FALLING:
            if (channel == EncChannel::CH1)
            {
                return TIM_CCER_CC1P;
            }
            if (channel == EncChannel::CH2)
            {
                return TIM_CCER_CC2P;
            }
            return TIM_CCER_CC1P | TIM_CCER_CC2P;
        default:
            return 0;
    }
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

    base_addr->CR1 = 0;
    base_addr->CNT = 0;
    base_addr->PSC = 0;
    base_addr->ARR =
        kMaxAutoReload;  // Set auto-reload to maximum for full range of counting

    // Configure both CH1 and CH2 for encoder mode
    if (channel == EncChannel::BOTH)
    {
        // Map CC1 and CC2 to TI1 and TI2 inputs for quadrature decoding.
        base_addr->CCMR1 &= ~(TIM_CCMR1_CC1S_Msk | TIM_CCMR1_CC2S_Msk);
        SetReg(&base_addr->CCMR1, kInputMappedOnTi, TIM_CCMR1_CC1S_Pos,
               kTimCcmrxCcsBitWidth);
        SetReg(&base_addr->CCMR1, kInputMappedOnTi, TIM_CCMR1_CC2S_Pos,
               kTimCcmrxCcsBitWidth);

        // Clear polarity bits for both channels
        base_addr->CCER &=
            ~(TIM_CCER_CC1P | TIM_CCER_CC1NP | TIM_CCER_CC2P | TIM_CCER_CC2NP);
        base_addr->CCER |= EncoderPolarityToCcer(settings.polarity, channel);
        base_addr->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E;

        // Set encoder mode (SMS bits)
        SetReg(&base_addr->SMCR, EncoderModeToSms(settings.mode),
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

            // Map CH1 capture to TI1.
            SetReg(&base_addr->CCMR1, kInputMappedOnTi, TIM_CCMR1_CC1S_Pos,
                   kTimCcmrxCcsBitWidth);

            base_addr->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP);
            base_addr->CCER |=
                EncoderPolarityToCcer(settings.polarity, EncChannel::CH1);
            base_addr->CCER |= TIM_CCER_CC1E;

            // Set the slave mode for CH1
            SetReg(&base_addr->SMCR, static_cast<uint32_t>(settings.slave_mode),
                   TIM_SMCR_SMS_Pos, kTimSmcrSmsBitWidth);

            break;
        case EncChannel::CH2:
            // Clear the CC2S bits to select the input for CH2
            base_addr->CCMR1 &= ~TIM_CCMR1_CC2S_Msk;

            // Map CH2 capture to TI2.
            SetReg(&base_addr->CCMR1, kInputMappedOnTi, TIM_CCMR1_CC2S_Pos,
                   kTimCcmrxCcsBitWidth);

            base_addr->CCER &= ~(TIM_CCER_CC2P | TIM_CCER_CC2NP);
            base_addr->CCER |=
                EncoderPolarityToCcer(settings.polarity, EncChannel::CH2);
            base_addr->CCER |= TIM_CCER_CC2E;

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
