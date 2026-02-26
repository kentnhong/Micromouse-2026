/**
 * @brief USART generic header file 
 * @author Bex Saw
 * @date 2/21/2026
*/

#pragma once
#include <cstdint>
#include <span>

namespace MM
{
class Usart
{
public:
    /**
    * @brief Receive (Read) data from USART
    * @param byte Byte of data to receive
    */
    virtual bool receive(uint8_t& byte) = 0;

    /**
    * @brief Send data to USART
    * @param txbuf Span containing data to be sent
    * @return true if write is successful, false otherwise
    */
    virtual bool send(std::span<const uint8_t> txbuf) = 0;
};
}  // namespace MM
