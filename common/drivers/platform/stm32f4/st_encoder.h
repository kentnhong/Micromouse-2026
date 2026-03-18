/**
* @file encoder.h
* @brief STM32F4 Encoder Driver header interface
* @date 3/17/2026
* @author Bex Saw
*/

#pragma once
#include "encoder.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{

/**
* @brief Encoder modes for STM32F4 Input Capture
* @note MODE_1: Counter counts on TI1FP1 edge depending on the level
        MODE_2: Counter counts on both TI1 and TI2 edges depending on the level of the other input
        MODE_3: Counter counts on both TI1 and TI2 edges depending on the level of the other input
*/
enum class EncMode : uint8_t
{
    MODE_1 = 0,
    MODE_2,
    MODE_3,
};

/**
* @brief Encoder channel options for STM32F4
* @note CH1: TIMx_CH1 is used for encoder input
        CH2: TIMx_CH2 is used for encoder input
*/
enum class EncChannel : uint8_t
{
    CH1 = 0,
    CH2,
    BOTH,  // (CH1 | CH2)
};

/**
* @brief Encoder polarity options for STM32F4
* @note RISING: Counter counts on rising edge
        FALLING: Counter counts on falling edge
        BOTH: Counter counts on both edges
*/
enum class EncInputPolarity : uint8_t
{
    RISING = 0,
    FALLING,
    BOTH,
};

/**
* @brief Encoder slave mode options for STM32F4
* @note DISABLED: Encoder is not used as a slave 
        MODE_1: Encoder is used as a slave in mode 1
        MODE_2: Encoder is used as a slave in mode 2 
        MODE_3: Encoder is used as a slave in mode 3 (T1 & T2 edges)
*/
enum class EncSlaveMode : uint8_t
{
    DISABLED = 0,
    MODE_1,
    MODE_2,
    MODE_3,
};

struct StEncoderSettings
{
    EncMode mode;
    EncChannel channel;
    EncInputPolarity polarity;
    EncSlaveMode slave_mode;
};

struct StEncoderParams
{
    TIM_TypeDef* base_addr;
    StEncoderSettings settings;
};

class HwEncoder : public Encoder
{
public:
    /**
    * @brief Constructor for HwEncoder
    * @param params_ - Parameters for initializing the encoder
    */
    explicit HwEncoder(const StEncoderParams& params_);

    /**
    * @brief Initializes the encoder with the specified settings
    */
    bool init();

    /**
    * @brief Read the number of ticks from the encoder
    * @return The number of ticks
    */
    bool get_ticks(int32_t ticks) const override;

    /**
    * @brief Resets the encoder ticks to zero
    * @return True if the reset was successful, false otherwise
    */
    bool reset_ticks(int32_t ticks) override;

private:
    TIM_TypeDef* base_addr;
    EncChannel channel;
    StEncoderSettings settings;
    int32_t current_ticks;
    int32_t prev_ticks;
};
}  // namespace Stmf4
}  // namespace MM