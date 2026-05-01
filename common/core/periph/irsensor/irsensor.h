/**
 * @file irsensor.h
 * @author Kent Hong
 * @brief IR Sensor state machine for Xupermouse
 */

#pragma once

#include <array>
#include <cstdint>
#include "adc.h"
#include "dma.h"
#include "gpio.h"

namespace MM
{

enum class IrStates : uint8_t
{
    SAMPLE_OFF_1 = 0,
    SAMPLE_OFF_2,
    EMITTER_ON,
    SETTLE_1,
    SAMPLE_ON_1,
    SAMPLE_ON_2,
    EMITTER_OFF,
    SETTLE_2
};

struct IrParams
{
    Adc& adc;
    Dma& dma;
    Gpio& emitter;
};

class IrSensor
{
public:
    /**
     * @brief Construct a new Ir Sensor object
     * 
     * @param params_ Struct with ADC, DMA, and GPIO objects to control the IR Sensor
     */
    explicit IrSensor(IrParams params_);

    /**
     * @brief Execute current IR Sensor state and move to next state
     * 
     * @return true State executed and incremented successfully, false otherwise.
     */
    bool update();

    /**
     * @brief Default destructor
     * 
     */
    ~IrSensor() = default;

private:
    /**
     * @brief Calculates true ADC value after taking the averages of the ambient and combined values then subtracting them
     *        ((combined[0] + combined[1]) / 2) - ((ambient[0] + ambient[1]) / 2)
     * 
     * @return true ADC value calculated successfully, false otherwise
     */
    bool calculate();

    IrStates current_state = IrStates::SAMPLE_OFF_1;
    Adc& adc;
    Dma& dma;
    Gpio& emitter;
    std::array<uint16_t, 2> ambient;  // ADC samples with ambient light
    std::array<uint16_t, 2>
        combined;           // ADC sample with ambient light + true IR
    uint16_t adc_true = 0;  // Final calculated ADC value with just true IR
};
};  // namespace MM