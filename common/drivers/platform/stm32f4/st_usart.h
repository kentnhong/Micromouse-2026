/**
* @file st_usart.h
* @brief USART driver for STM32F4 header file
* @author Bex Saw
* @date 2/21/2026
*/

#pragma once
#include <cstdint>
#include <span>
#include "stm32f4xx.h"
#include "usart.h"

namespace MM
{
namespace Stmf4
{

// USART setting for how many samples for one bit
enum class UsartOversample : uint8_t
{
    X16 = 0, 
    X8
};

enum class UsartSampleMode : uint8_t
{
    MAJORITY = 0,
    SINGLE
};

struct StUsartSettings
{
    UsartOversample oversample; 
    UsartSampleMode sample_mode;
};

struct StUsartParams
{
    USART_TypeDef* base_addr;
    uint32_t clock_freq;
    uint32_t baud_rate;
    StUsartSettings settings;
};

class StUsart : public Usart
{
private:
    USART_TypeDef* base_addr;
    StUsartSettings settings;
    uint32_t clock_freq;
    uint32_t baud_rate;

public:
    /**
    * @brief Construct a new StUsart object
    * @param params_ Usart struct containing params of base addr, clock freq, baud rate, and usart settings
    */
    explicit StUsart(StUsartParams& params_);

    /**
     * @brief Get the USART clock frequency
     * @return uint32_t Clock frequency in Hz
     */
    uint32_t get_clock_freq() const { return clock_freq; }

    /**
    * @brief Receive (Read) data from USART
    * @param byte Byte of data to receive
    */
    bool receive(uint8_t& byte) override;

    /**
    * @brief Send data to USART
    * @param txbuf Span containing data to be sent
    * @return true if write is successful, false otherwise
    */
    bool send(std::span<const uint8_t> txbuf) override;

    /**
    * @brief Initializes USART peripheral and its tx and rx pins
    * @return true if initialization is successful, false otherwise
    */
    bool init();

    /**
     * @brief Get the base_addr of the UART object
     * @note This is for use in the USART2 IRQ handler to check the SR & DR registers
     * @return USART_TypeDef* 
     */
    USART_TypeDef* get_addr();
};
}  // namespace Stmf4
}  // namespace MM
