

#pragma once
#include <stdint.h>
#include <cstddef>
#include "delay.h"
#include "i2c.h"
#include "mcu_support/stm32/f4xx/stm32f4xx.h"
#include "stm32f411xe.h"

namespace MM
{
namespace Stmf4
{

struct StI2cParams
{
    I2C_TypeDef* base_addr;
    uint16_t ccr;   // Clock control register value
    uint8_t trise;  // Maximum rise time register value
};

class HwI2c : public I2c
{

public:
    explicit HwI2c(const StI2cParams& params);

    /**
   * @brief Initialize I2C peripheral
   * @return true if successful, false otherwise
   */
    bool init();

    /*
    This function are polling based I2C read and write functions
    Because of F4 design, it's not modern so we have to do START, ADDR, DATA, STOP manually
    */

    /**
    * @brief Read data from external device that uses 8-bit memory addresses
    * @param data block of memory to read data into from the bus
    * @param reg_addr data register of external device to read from
    */
    bool mem_read(uint8_t* data, size_t len, const uint8_t reg_addr,
                  uint8_t dev_addr) override;

    /**
    * @brief Writes data to external device that uses 8-bit memory addresses
    * @param data block of memory storing data to write into the bus
    * @param reg_addr data register of external device to write to
    */
    bool mem_write(const uint8_t* data, size_t len, const uint8_t reg_addr,
                   uint8_t dev_addr) override;

    /**
    * @brief Read raw data from an I2C bus
    * @param data block of memory to read data into from the bus
    * @param dev_addr address of target device
    */
    bool read(uint8_t* data, size_t len, uint8_t dev_addr) override;

    /**
    * @brief Write raw data to an I2C bus
    * @param data block of memory to write data into the bus
    * @param dev_addr address of target device
    */
    bool write(const uint8_t* data, size_t len, uint8_t dev_addr) override;

private:
    I2C_TypeDef* _base_addr;
    uint16_t _ccr;
    uint8_t _trise;
};
}  // namespace Stmf4
}  // namespace MM