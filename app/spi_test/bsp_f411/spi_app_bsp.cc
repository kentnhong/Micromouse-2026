#include "board.h"
#include "gpio_cs.h"
#include "st_gpio.h"
#include "st_spi.h"
#include "stm32f4xx.h"
namespace MM
{
// Make CS GPIO Register Config Settings
MM::Stmf4::StGpioSettings cs_gpio_settings{
    MM::Stmf4::GpioMode::GPOUT, MM::Stmf4::GpioOtype::PUSH_PULL,
    MM::Stmf4::GpioOspeed::VERY_HIGH, MM::Stmf4::GpioPupd::NO_PULL, 0};
MM::Stmf4::StGpioParams cs_params{4, GPIOA, cs_gpio_settings};
MM::Stmf4::HwGpio cs_gpio{cs_params};

// Make SPI Register Config Settings
MM::Stmf4::StSpiSettings spi_settings{
    MM::Stmf4::SpiBaudRate::FPCLK_2, MM::Stmf4::SpiBusMode::MODE1,
    MM::Stmf4::SpiBitOrder::MSB, MM::Stmf4::SpiRxThreshold::FIFO_8bit};
MM::Stmf4::HwSpi spi1{SPI1, spi_settings};

// Make GPIO Register Config Settings
MM::Stmf4::StGpioSettings gpio_settings{
    MM::Stmf4::GpioMode::AF, MM::Stmf4::GpioOtype::PUSH_PULL,
    MM::Stmf4::GpioOspeed::VERY_HIGH, MM::Stmf4::GpioPupd::NO_PULL, 5};
MM::Stmf4::StGpioParams sck_params{5, GPIOA, gpio_settings};
MM::Stmf4::StGpioParams miso_params{6, GPIOA, gpio_settings};
MM::Stmf4::StGpioParams mosi_params{7, GPIOA, gpio_settings};
// Make GPIO Spi pins
MM::Stmf4::HwGpio sck{sck_params};
MM::Stmf4::HwGpio miso{miso_params};
MM::Stmf4::HwGpio mosi{mosi_params};

// Create Chip Select Object to manual toggle cs pin
MM::GpioChipSelect chip_select{cs_gpio};

Board board{.cs = chip_select, .spi1 = spi1};

// Initialize SPI BSP
bool BSP_Init()
{
    // Reference status
    bool result = true;

    // Enable GPIOA clock (STM32F4 uses AHB1ENR)
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // Enable SPI clock
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

    // Init SPI periph and check if it was successful
    result = result && spi1.init();

    // Init Spi pins
    result = result && sck.init();
    result = result && miso.init();
    result = result && mosi.init();

    // Init CS pin
    result = result && cs_gpio.init();
    chip_select.cs_enable();

    return result;
}

Board& Get_Board()
{
    return board;
}
}  // namespace MM
