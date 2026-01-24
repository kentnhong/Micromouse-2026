#include <cstdint>
#include "board.h"
#include "st_gpio.h"
#include "st_i2c.h"

namespace MM
{
// SCL pin config (PB8)

Stmf4::StGpioSettings scl_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::OPEN_DRAIN, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP};

Stmf4::StGpioParams scl_params{8, GPIOB, scl_settings};  // PB8

Stmf4::StGpioSettings sda_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::OPEN_DRAIN, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP};

Stmf4::StGpioParams sda_params{9, GPIOB, sda_settings};  // PB9

Stmf4::StI2cParams i2c_params{
    I2C1, 0x10909CEC};  // Timing register for 100kHz @ 16MHz PCLK1

Stmf4::HwI2c i2c(i2c_params);
Stmf4::HwGpio scl(scl_params);
Stmf4::HwGpio sda(sda_params);

Board board{.i2c = i2c};

bool bsp_init()
{
    // Enable peripheral clocks for I2C1 and GPIOB
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    // Initialize I2C and pins
    bool ret = true;

    ret = ret && sda.init();
    ret = ret && scl.init();
    ret = ret && i2c.init();
    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM