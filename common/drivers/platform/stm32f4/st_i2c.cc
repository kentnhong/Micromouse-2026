#include "st_i2c.h"

/**
* The implementation in polling based I2C read and write functions
* We have data, len, reg_addr, dev_addr to do I2C read and write
* @date 1/23/2026
*/

namespace MM
{
namespace Stmf4
{
HwI2c::HwI2c(const StI2cParams& params) : _base_addr{params.base_addr}
{
}

bool HwI2c::init()
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Reset peripheral
    _base_addr->CR1 &= ~I2C_CR1_PE;

    // Enable peripheral
    _base_addr->CR1 |= I2C_CR1_PE;

    return true;
}

bool HwI2c::mem_read(uint8_t* data, size_t len, const uint8_t reg_addr,
                     uint8_t dev_addr)
{
    if (_base_addr == nullptr)
    {
        return false;
    }

    // Check if init was called
    if (!(_base_addr->CR1 & I2C_CR1_PE))
    {
        return false;
    }

    // F4: Wait for bus idle
    if (_base_addr->SR2 & I2C_SR2_BUSY)
        return false;

    // START, send address (write)
    _base_addr->CR1 |= I2C_CR1_START;
    while (!(_base_addr->SR1 & I2C_SR1_SB)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 0;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR2;
    while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);
    _base_addr->DR = reg_addr;
    while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);

    // Repeated START, send address (read)
    _base_addr->CR1 |= I2C_CR1_START;
    while (!(_base_addr->SR1 & I2C_SR1_SB)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 1;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR2;
    for (size_t i = 0; i < len; ++i)
    {
        while (!(_base_addr->SR1 & I2C_SR1_RXNE)) MM::Utils::DelayUs(1);
        data[i] = _base_addr->DR;
    }
    _base_addr->CR1 |= I2C_CR1_STOP;
    return true;
}

bool HwI2c::mem_write(const uint8_t* data, size_t len, const uint8_t reg_addr,
                      uint8_t dev_addr)
{
    if (_base_addr == nullptr)
        return false;

    if (!(_base_addr->CR1 & I2C_CR1_PE))
        return false;

    // Wait for bus idle
    if (_base_addr->SR2 & I2C_SR2_BUSY)
        return false;

    // START, send address (write)
    _base_addr->CR1 |= I2C_CR1_START;
    while (!(_base_addr->SR1 & I2C_SR1_SB)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 0;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR2;
    while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);
    _base_addr->DR = reg_addr;
    while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);

    // Write data
    for (size_t i = 0; i < len; ++i)
    {
        _base_addr->DR = data[i];
<<<<<<< HEAD
        timeout = 10000;
        while (!(_base_addr->SR1 & I2C_SR1_TXE) && --timeout);
        if (timeout == 0)
            return false;
    }

    // Wait for transfer finished
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_BTF) && --timeout);
    if (timeout == 0)
        return false;
=======
        while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);
    }

    // Wait for transfer finished
    while (!(_base_addr->SR1 & I2C_SR1_BTF)) MM::Utils::DelayUs(1);
>>>>>>> 1bf1c97 (working blink + i2c need testing)
    _base_addr->CR1 |= I2C_CR1_STOP;
    return true;
}

bool HwI2c::write(const uint8_t* data, size_t len, uint8_t dev_addr)
{
    if (_base_addr == nullptr)
        return false;

    if (!(_base_addr->CR1 & I2C_CR1_PE))
        return false;

<<<<<<< HEAD
    // Wait for bus idle, try to recover if stuck
    int timeout = 10000;
    while ((_base_addr->SR2 & I2C_SR2_BUSY) && --timeout);
    if (timeout == 0)
    {
        // Try to recover bus
        _base_addr->CR1 |= I2C_CR1_STOP;
        MM::Utils::DelayUs(10);
        if (_base_addr->SR2 & I2C_SR2_BUSY)
            return false;
    }

    // START, send address (write)
    _base_addr->CR1 |= I2C_CR1_START;
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_SB) && --timeout);
    if (timeout == 0)
        return false;
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 0;
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR) && --timeout);
    if (timeout == 0)
        return false;
=======
    // Wait for bus idle
    if (_base_addr->SR2 & I2C_SR2_BUSY)
        return false;

    // START, send address (write)
    _base_addr->CR1 |= I2C_CR1_START;
    while (!(_base_addr->SR1 & I2C_SR1_SB)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 0;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR)) MM::Utils::DelayUs(1);
>>>>>>> 1bf1c97 (working blink + i2c need testing)
    (void)_base_addr->SR2;

    // Write data
    for (size_t i = 0; i < len; ++i)
    {
        _base_addr->DR = data[i];
<<<<<<< HEAD
        timeout = 10000;
        while (!(_base_addr->SR1 & I2C_SR1_TXE) && --timeout);
        if (timeout == 0)
            return false;
    }

    // Wait for transfer finished
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_BTF) && --timeout);
    if (timeout == 0)
        return false;
=======
        while (!(_base_addr->SR1 & I2C_SR1_TXE)) MM::Utils::DelayUs(1);
    }

    // Wait for transfer finished
    while (!(_base_addr->SR1 & I2C_SR1_BTF)) MM::Utils::DelayUs(1);
>>>>>>> 1bf1c97 (working blink + i2c need testing)
    _base_addr->CR1 |= I2C_CR1_STOP;
    return true;
}

bool HwI2c::read(uint8_t* data, size_t len, uint8_t dev_addr)
{
    if (_base_addr == nullptr)
        return false;

    if (!(_base_addr->CR1 & I2C_CR1_PE))
        return false;

<<<<<<< HEAD
    // Wait for bus idle, try to recover if stuck
    int timeout = 10000;
    while ((_base_addr->SR2 & I2C_SR2_BUSY) && --timeout);
    if (timeout == 0)
    {
        // Try to recover bus
        _base_addr->CR1 |= I2C_CR1_STOP;
        MM::Utils::DelayUs(10);
        if (_base_addr->SR2 & I2C_SR2_BUSY)
            return false;
    }

    // START, send address (read)
    _base_addr->CR1 |= I2C_CR1_START;
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_SB) && --timeout);
    if (timeout == 0)
        return false;
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 1;
    timeout = 10000;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR) && --timeout);
    if (timeout == 0)
        return false;
    // For multi-byte read, handle ACK/NACK for last byte
    if (len > 1)
    {
        _base_addr->CR1 |= I2C_CR1_ACK;
    }
    else
    {
        _base_addr->CR1 &= ~I2C_CR1_ACK;
    }
    (void)_base_addr->SR2;
    for (size_t i = 0; i < len; ++i)
    {
        // For last byte, clear ACK before reading
        if (i == len - 1)
        {
            _base_addr->CR1 &= ~I2C_CR1_ACK;
        }
        timeout = 10000;
        while (!(_base_addr->SR1 & I2C_SR1_RXNE) && --timeout);
        if (timeout == 0)
            return false;
        data[i] = _base_addr->DR;
    }
    _base_addr->CR1 |= I2C_CR1_STOP;
    // Restore ACK for next transfer
    _base_addr->CR1 |= I2C_CR1_ACK;
=======
    // Wait for bus idle
    if (_base_addr->SR2 & I2C_SR2_BUSY)
        return false;

    // START, send address (read)
    _base_addr->CR1 |= I2C_CR1_START;
    while (!(_base_addr->SR1 & I2C_SR1_SB)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR1;
    _base_addr->DR = (dev_addr << 1) | 1;
    while (!(_base_addr->SR1 & I2C_SR1_ADDR)) MM::Utils::DelayUs(1);
    (void)_base_addr->SR2;

    for (size_t i = 0; i < len; ++i)
    {
        while (!(_base_addr->SR1 & I2C_SR1_RXNE)) MM::Utils::DelayUs(1);
        data[i] = _base_addr->DR;
    }
    _base_addr->CR1 |= I2C_CR1_STOP;
>>>>>>> 1bf1c97 (working blink + i2c need testing)
    return true;
}

}  // namespace Stmf4
}  // namespace MM