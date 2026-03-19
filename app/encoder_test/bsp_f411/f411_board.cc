#include "board.h"
#include "st_encoder.h"
#include "st_gpio.h"

namespace MM
{

// Input Configuration for Timer (TIM2 encoder: PB3=CH1, PB10=CH2, both AF1)
Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

const Stmf4::StGpioParams enc_input_params_1{
    3, GPIOB, enc_input_settings};  // PB3 (TIM2_CH1)
const Stmf4::StGpioParams enc_input_params_2{
    10, GPIOB, enc_input_settings};  // PB10 (TIM2_CH2)

// Encoder Config (TIM2, BOTH Channel)
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::BOTH, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};

// Create Encoder GPIO & Encoder object
Stmf4::HwGpio encoder_ch1(enc_input_params_1);
Stmf4::HwGpio encoder_ch2(enc_input_params_2);
Stmf4::HwEncoder encoder(encoder_params);

Board board{.encoder = encoder};

bool bsp_init()
{
    // Enable peripheral clocks
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
