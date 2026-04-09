#include "board.h"
#include "st_encoder.h"
#include "st_gpio.h"

namespace MM
{

// GPIO settings for encoder input (change AF/INPUT)

Stmf4::StGpioSettings encoder_gpio_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

const Stmf4::StGpioParams enc_input_params_1{0, GPIOA,
                                             encoder_gpio_settings};  // PA0 channel A
const Stmf4::StGpioParams enc_input_params_2{1, GPIOA,
                                             encoder_gpio_settings};  // PA1 Channel B

// Encoder Config (TIM2, BOTH Channel)
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};


// Create Encoder GPIO & Encoder object
Stmf4::HwGpio encoder_ch1(enc_input_params_1);
Stmf4::HwGpio encoder_ch2(enc_input_params_2);
Stmf4::HwEncoder encoder(encoder_params);

Board board{.encoder = encoder, .enc_ch1 = encoder_ch1, .enc_ch2 = encoder_ch2};

bool bsp_init()
{
    // Enable peripheral clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

    // Intialize encoder and pins
    bool ret = true;

    ret = ret && encoder_ch1.init();
    ret = ret && encoder_ch2.init();
    ret = ret && encoder.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
