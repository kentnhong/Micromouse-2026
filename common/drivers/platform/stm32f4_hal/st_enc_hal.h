/**
 * @file st_enc_hal.h
 * @brief STM32F4 HAL-based Encoder driver header interface
 * @author Bex Saw
 */

#pragma once
#include <cstdint>
#include "encoder.h"
#include "stm32f411xe.h"
#include "stm32f4xx_hal.h"

namespace MM
{
namespace Stmf4Hal
{

enum class EncMode : uint8_t
{
    MODE_1 = 1,
    MODE_2 = 2,
    MODE_3 = 3,
};

enum class EncChannel : uint8_t
{
    CH1 = 0,
    CH2,
    BOTH,
};

enum class EncInputPolarity : uint8_t
{
    RISING = 0,
    FALLING,
    BOTH,
};

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

class HwEncoderHal : public Encoder
{
public:
    explicit HwEncoderHal(const StEncoderParams& params);

    /**
    * @brief Initializes the encoder hardware and starts counting
    * @return true if initialization was successful, false otherwise
    */
    bool init();

    /**
    * @brief Retrieves the current tick count from the encoder
    * @return The current tick count as a signed 32-bit integer
    */
    int32_t get_ticks() override;

    /**
    * @brief Resets the tick count to zero
    * @return true if reset was successful, false otherwise
    */
    bool reset_ticks() override;

    /**
    * @brief Initializes the cycle counter
    * @return true if initialization was successful, false otherwise
    */
    bool init_cycle_counter() override;

    /**
    * @brief Retrieves the current time in cycles
    * @return The current time in cycles as a 32-bit unsigned integer
    */
    uint32_t get_time_cycles() override;

    /**
    * @brief Retrieves the number of cycles per microsecond based on the system clock
    * @return The number of cycles per microsecond as a 32-bit unsigned integer
    */
    uint32_t cycles_per_us() override;

    /**
    * @brief Measures encoder statistics such as ticks per revolution and distance per tick
    * @param sample_cycles The duration of the sampling period in cycles
    * @param ticks_per_output_rev The number of encoder ticks per output revolution
    * @param cm_per_tick The distance in centimeters per encoder tick
    * @return An EncoderStats struct containing the measured statistics
    */
    EncoderStats measure_encoder_stats(uint32_t sample_cycles,
                                       float ticks_per_output_rev,
                                       float cm_per_tick) override;

private:
    TIM_TypeDef* base_addr;
    EncChannel channel;
    StEncoderSettings settings;

    int32_t current_ticks;

    TIM_HandleTypeDef htim_{};

    TIM_HandleTypeDef& hal();

    void configure_encoder_mode();
    int32_t sign_extend_cnt() const;
};

}  // namespace Stmf4Hal
}  // namespace MM
