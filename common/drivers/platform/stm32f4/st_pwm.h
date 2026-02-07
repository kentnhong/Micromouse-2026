/**
* @file st_pwm.h
* @brief STM32F4 PWM driver header interface
* @date 2/6/2026
* @author Bex Saw
*/

#pragma once
#include "pwm.h"
#include "stm32f4xx.h"
#include <cstdint>

namespace MM
{

namespace Stmf4
{
/**
* @brief STM32F4 specific implementation of the Pwm interface
* @note EDGE_ALIGNED: counter counts up or down depending on DIR bit; flag set when CNT == CCRx
*       CENTER_ALIGNED_DOWN: counter counts up and down; flag set when counting down
*       CENTER_ALIGNED_UP: counter counts up and down; flag set when counting up
*       CENTER_ALIGNED_UP_DOWN : counter counts up and down; flag set when counting up and down
*/
enum class PwmMode : uint8_t 
{
    EDGE_ALIGNED = 0,
    CENTER_ALIGNED_DOWN,
    CENTER_ALIGNED_UP,
    CENTER_ALIGNED_UP_DOWN
};

/**
* @brief Output compare modes for STM32F4 PWM
* @note PWM_MODE_1: active until match, then inactive
        PWM_MODE_2: inactive until match, then active
*/
enum class PwmOutputMode : uint8_t
{
    PWM_MODE_1 = 6,   
    PWM_MODE_2 = 7   
};

/**
* @brief Polarity options for STM32F4 PWM
* @note ACTIVE_HIGH: output is active when high
        ACTIVE_LOW: output is active when low
*/

enum class PwmDir : uint8_t
{
    ACTIVE_HIGH = 0,
    ACTIVE_LOW
};

/**
* @brief Configuration structure for STM32F4 PWM settings
*/
struct StPwmSettings 
{
    PwmMode mode;             
    PwmOutputMode outputMode;  
    PwmDir polarity;           
};

struct StPwmParams
{
    TIM_TypeDef* base_addr;  
    uint8_t channel;    
    StPwmSettings settings;
};

class HwPwm : public Pwm
{
public:
    /**
    * @brief Constructor for HwPwm
    * @param params_ Configuration parameters for the PWM instance
    */
    HwPwm(const StPwmParams& params_);

    /**
    * @brief Initializes the PWM timer with the specified settings
    */
    bool init() override;

    /**
    * @brief Sets the frequency of the PWM timer
    * @param frequency Desired frequency in Hz
    * @return true if the frequency was set successfully, false otherwise
    */
    bool setFrequency(uint32_t frequency) override;

    /**
    * @brief Sets the duty cycle of the PWM signal
    * @param dutyCycle Desired duty cycle as a percentage (0-100)
    * @return true if the duty cycle was set successfully, false otherwise
    * @note     Edge-aligned: 42 MHz  / (PSC + 1)
                Center-aligned: 21 MHz  / (PSC + 1)
    */
    bool setDutyCycle(uint8_t dutyCycle) override;

private:
    TIM_TypeDef* base_addr; 
    uint8_t channel; 
    StPwmParams params;
    uint32_t currentFrequency;
    uint8_t currentDutyCycle;

};
} // namespace Stmf4
} // namespace MM