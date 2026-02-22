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
class StUsart : public Usart
{
private:
    USART_TypeDef* base_addr;
    uint16_t uartdiv;

public:
    /**
    * @brief Construct a new StUsart object
    * @param base_addr Base address of the USART peripheral
    * @param sys_clk System clock frequency in Hz
    * @param clock_freq Desired USART clock frequency in Hz
    */
    explicit StUsart(USART_TypeDef* base_addr, uint32_t sys_clk,
                     uint32_t clock_freq);

    /**
    * @brief Receive (Read) data from USART
    * @param data Pointer to buffer where received data will be stored
    * @param length Length of data to read
    * @return true if read is successful, false otherwise
    */
    bool receive(uint8_t* data, size_t length) override;

    /**
    * @brief Transfer (transmit) data to USART
    * @param txbuf Span containing data to be sent
    * @return true if write is successful, false otherwise
    */
    bool transfer(std::span<const uint8_t> txbuf) override;

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
