
/**
 * @file st_spi.cc
 * @author Kent Hong
 * @brief SPI Driver class implementation for STM32F411xx
 * @date 2025-09-30 (F4 update 2026)
 */

#include "st_spi.h"
#include <cstddef>
#include "common/drivers/time/delay.h"
#include "mcu_support/stm32/f4xx/stm32f4xx.h"

namespace MM
{
namespace Stmf4
{

/**
 * @brief Construct a new HwSpi object
 *
 * @param instance_ The SPI peripheral being used
 * @param settings_ The SPI control register settings
 *
 */
HwSpi::HwSpi(SPI_TypeDef* instance_, StSpiSettings& settings_)
    : instance(instance_), settings(settings_)
{
}

/**
 * @brief Read data from a slave device.
 * 
 * @param rx_data 8 byte array to store read data.
 * @param buffer_len Size of array.
 * @return true 
 * @return false 
 */
bool HwSpi::read(std::span<uint8_t> rx_data)
{

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (size_t i = 0; i < rx_data.size(); i++)
    {
        int timeout = 1000;
        while (!(instance->SR & SPI_SR_TXE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        *(volatile uint8_t*)&instance->DR = 0x00;

        timeout = 1000;
        while (!(instance->SR & SPI_SR_RXNE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        rx_data[i] = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief write data to a slave device.
 * 
 * @param tx_data 8 byte array of the data to be sent.
 * @param buffer_len Size of array
 * @return true 
 * @return false 
 */
bool HwSpi::write(std::span<uint8_t> tx_data)
{

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    for (size_t i = 0; i < tx_data.size(); i++)
    {
        int timeout = 1000;
        while (!(instance->SR & SPI_SR_TXE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        *(volatile uint8_t*)&instance->DR = tx_data[i];

        timeout = 1000;
        while (!(instance->SR & SPI_SR_RXNE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        (void)(*(volatile uint8_t*)&instance->DR);
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief read and Write data to a slave device.
 * 
 * @param tx_data 8 byte array of the data to be sent.
 * @param rx_data 8 byte array to store read data.
 * @param buffer_len Size of arrays
 * @return true 
 * @return false 
 */
bool HwSpi::seq_transfer(std::span<uint8_t> tx_data, std::span<uint8_t> rx_data)
{
    // Check if SPI is enabled
    if (!(instance->CR1 & SPI_CR1_SPE))
    {
        return false;
    }

    // Check if SPI is already in communication
    if (instance->SR & SPI_SR_BSY)
    {
        return false;
    }

    /*
     * First send all tx bytes and clear the RXNE flag for each transmitted
     * byte (we don't use these intermediate bytes for flash commands).
     */
    for (size_t i = 0; i < tx_data.size(); i++)
    {
        int timeout = 1000;
        while (!(instance->SR & SPI_SR_TXE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        *(volatile uint8_t*)&instance->DR = tx_data[i];

        timeout = 1000;
        while (!(instance->SR & SPI_SR_RXNE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        (void)(*(volatile uint8_t*)&instance->DR);
    }

    /*
     * Now generate clock pulses by sending dummy bytes to read the expected
     * response from the slave into rx_data[0..rx_len-1].
     */
    for (size_t j = 0; j < rx_data.size(); j++)
    {
        int timeout = 1000;
        while (!(instance->SR & SPI_SR_TXE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        *(volatile uint8_t*)&instance->DR = 0x00;

        timeout = 1000;
        while (!(instance->SR & SPI_SR_RXNE) && --timeout > 0)
        {
            MM::Utils::DelayUs(1);
        }
        if (timeout == 0)
            return false;

        rx_data[j] = *(volatile uint8_t*)&instance->DR;
    }

    // Wait until transmission is complete
    while (instance->SR & SPI_SR_BSY)
    {
    }

    return true;
}

/**
 * @brief Initializes SPI peripheral and its sck, mosi, miso, and nss pins
 *
 */
bool HwSpi::init()
{
    // TODO: Runtime validation of enum values (will change to compile time checks in the future and maybe make a private function for these checks)
    if (static_cast<uint8_t>(settings.baudrate) > 7)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.busmode) > 3)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.order) > 1)
    {
        return false;
    }
    if (static_cast<uint8_t>(settings.threshold) > 1)
    {
        return false;
    }

    /* 
     * Set to Master Mode (Keep in master mode unless we want our STM32l4xx to be
     * used as a slave device). Master Mode sets our STM32l4xx to act as the
     * master device to initiate/end SPI communication and drive the clock signal. 
     */
    instance->CR1 |= SPI_CR1_MSTR;

    /* 
     * Enable Full-Duplex Mode (Can send and receive data simultaneously through
     * MOSI and MISO) 
     */
    instance->CR1 &= ~(SPI_CR1_RXONLY);

    // Configure SPI sck Baudrate
    SetReg(&instance->CR1, uint32_t(settings.baudrate), 3, 3);

    // Configure the SPI Bus Mode
    SetReg(&instance->CR1, uint32_t(settings.busmode), 0, 2);

    // Configure Bit order
    SetReg(&instance->CR1, uint32_t(settings.order), 7, 1);

    /* 
     * Determine FIFO reception threshold to see how many bits in RX Buffer
     * triggers an RXNE event
     */
    SetReg(&instance->CR2, uint32_t(settings.threshold), 12, 1);

    // Configure the SPI data size to 8 bits
    instance->CR1 &= ~(SPI_CR1_DFF);  // 8-bit data frame format
    //instance->CR2 |= SPI_CR2_FRXTH; // 16-bit FIFO threshold

    // Use software slave management and toggle SSI = 1 to pull NSS to high
    // Toggle SSI = 0 when we want to select the slave device for data transfer
    instance->CR1 |= SPI_CR1_SSM;
    instance->CR1 |= SPI_CR1_SSI;

    // Enable SPI peripheral
    instance->CR1 |= SPI_CR1_SPE;

    return true;
}
}  // namespace Stmf4
}  // namespace MM