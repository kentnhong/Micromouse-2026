#include <cstdint>
#include "../board.h"
#include "bno055_imu.h"
#include "st_gpio.h"
#include "st_i2c.h"

namespace MM
{
// Static pin/I2C/IMU objects for F4 board
Stmf4::StGpioSettings gpio_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::OPEN_DRAIN, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 4};

Stmf4::StGpioParams scl_params{8, GPIOB, gpio_settings};
Stmf4::StGpioParams sda_params{9, GPIOB, gpio_settings};

Stmf4::HwGpio scl(scl_params);
Stmf4::HwGpio sda(sda_params);

static constexpr uint16_t CCR_100KHZ = 0x1F4;
static constexpr uint16_t TRISE_100KHZ = 0x2B;

Stmf4::StI2cParams i2c_params{I2C1, CCR_100KHZ, TRISE_100KHZ};
Stmf4::HwI2c i2c(i2c_params);

Stmf4::StGpioSettings rst_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 0};

Stmf4::StGpioParams rst_params{0, GPIOA, rst_settings};
Stmf4::HwGpio rst(rst_params);

Bno055 imu(static_cast<MM::I2c&>(i2c), Bno055::ADDR_PRIMARY);
Board board{.imu = imu};

bool bsp_init()
{
    // Enable GPIOB and I2C1 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;

    scl.init();
    sda.init();
    i2c.init();
    rst.init();

    // BNO055 reset sequence
    rst.set(0);  // Hold BNO055 in reset
    MM::Utils::DelayMs(10);
    rst.set(1);               // Release reset
    MM::Utils::DelayMs(650);  // Wait for BNO055 to boot

    imu.init();
    uint8_t chip_id = 0;
    bool ok = imu.get_chip_id(chip_id);
    if (!ok || chip_id != 0xA0)
    {
        // IMU not detected or wrong chip ID
        return false;
    }
    return true;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
