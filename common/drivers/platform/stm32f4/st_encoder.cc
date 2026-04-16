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

bool HwEncoder::init_cycle_counter()
{
    // Enable TRC
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;

    // Reset the cycle counter
    DWT->CYCCNT = 0;

    // Enable the cycle counter
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

    return (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) != 0;
}

uint32_t HwEncoder::get_time_cycles()
{
    return DWT->CYCCNT;
}

uint32_t HwEncoder::cycles_per_us()
{
    return SystemCoreClock / 1'000'000u;
}

EncoderStats HwEncoder::measure_encoder_stats(uint32_t sample_cycles,
                                              float ticks_per_output_rev,
                                              float cm_per_tick)
{
    int32_t start_ticks = get_ticks();
    uint32_t start_time = get_time_cycles();

    while ((get_time_cycles() - start_time) < sample_cycles)
    {
    }

    uint32_t end_time = get_time_cycles();
    uint32_t delta_cycles = end_time - start_time;

    uint32_t cycles_per_microsecond = cycles_per_us();
    if (delta_cycles == 0 || cycles_per_microsecond == 0 ||
        ticks_per_output_rev == 0.0f)
    {
        return {0.0f, 0.0f, 0.0f, 0.0f, 0};
    }

    int32_t end_ticks = get_ticks();
    int32_t delta_ticks = end_ticks - start_ticks;

    float delta_time_us = static_cast<float>(delta_cycles) /
                          static_cast<float>(cycles_per_microsecond);

    // Convert us to seconds for velocity
    float delta_time_sec = delta_time_us / 1'000'000.0f;

    // Calculate velocity and distance
    float ticks_per_sec = static_cast<float>(delta_ticks) / delta_time_sec;

    // RPM = (60 sec / rev) * (ticks / sec) / (ticks / rev)
    // Expected: (60 / 1) * (ticks/sec) / 180 ticks/rev = ticks/sec * 0.3333 RPM per tick/sec = # RPM
    // IF 6000 Ticks/sec => 6000 * 0.3333 = 2000 RPM, which is reasonable
    float rpm = (60.0f / ticks_per_output_rev) * ticks_per_sec;

    // Velocity in cm/s = (cm/tick) * (ticks/sec)
    // Expected: 0.0244 cm/tick * ticks/sec = # velocity in cm/s
    float velocity_cm_per_sec = cm_per_tick * ticks_per_sec;

    // Distance in cm = (cm/tick) * (delta ticks)
    // Expected: 0.0244 cm/tick * delta ticks = # distance in cm
    float distance_cm = cm_per_tick * static_cast<float>(delta_ticks);

    return {rpm, velocity_cm_per_sec, distance_cm, delta_time_sec, delta_ticks};
}

}  // namespace Stmf4
}  // namespace MM
