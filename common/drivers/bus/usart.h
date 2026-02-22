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
    * @param data Pointer to buffer where received data will be stored
    * @param len Length of data to read
    * @return true if read is successful, false otherwise
    */
    virtual bool receive(uint8_t* data, size_t len) = 0;

    /**
    * @brief Transfer (send) data to USART
    * @param txbuf Span containing data to be sent
    * @return true if write is successful, false otherwise
    */
    virtual bool send(std::span<const uint8_t> txbuf) = 0;
};
}  // namespace MM
