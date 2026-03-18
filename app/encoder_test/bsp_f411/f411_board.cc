#include "board.h"
#include "st_encoder.h"
#include "st_gpio.h"

namespace MM
{

// Input Configuration for Timer
Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::GPI, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 0};

const Stmf4::StGpioParams enc_input_params{5, GPIOB, enc_input_settings};

// Encoder Config (TIM2, BOTH Channel)
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::BOTH, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};

// Create Encoder GPIO & Encoder object
Stmf4::HwGpio encoder_output(enc_input_params);
Stmf4::HwEncoder encoder(encoder_params);

Board board{.encoder = encoder};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Intialize encoder and pins
    bool ret = true;

    ret = ret && encoder_output.init();
    ret = ret && encoder.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
