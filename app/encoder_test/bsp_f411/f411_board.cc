#include "board.h"
#include "st_encoder.h"
#include "st_gpio.h"

namespace MM
{

// Input configuration for TIM2 encoder mode using TI1/TI2 on PA0/PA1 (AF1).
Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

const Stmf4::StGpioParams enc_input_params_1{
    0, GPIOA, enc_input_settings};  // PA0 (TIM2_CH1)
const Stmf4::StGpioParams enc_input_params_2{
    1, GPIOA, enc_input_settings};  // PA1 (TIM2_CH2)

/* Dummy inputs so we can test loopback */

// Dummy quadrature outputs for loopback testing.
// Stmf4::StGpioSettings dummy_output_settings{
//    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
//    Stmf4::GpioPupd::NO_PULL, 0};

// const Stmf4::StGpioParams dummy_output_params_1{
//     4, GPIOB, dummy_output_settings};  // PB4 -> jumper to PA0
// const Stmf4::StGpioParams dummy_output_params_2{
//     5, GPIOB, dummy_output_settings};  // PB5 -> jumper to PA1

// Encoder Config (TIM2, BOTH Channel)
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};

// Create Encoder GPIO & Encoder object
Stmf4::HwGpio encoder_ch1(enc_input_params_1);
Stmf4::HwGpio encoder_ch2(enc_input_params_2);
//Stmf4::HwGpio dummy_ch1(dummy_output_params_1);
//Stmf4::HwGpio dummy_ch2(dummy_output_params_2);
Stmf4::HwEncoder encoder(encoder_params);

Board board{.encoder = encoder};

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
    // ret = ret && dummy_ch1.init();
    // ret = ret && dummy_ch2.init();
    ret = ret && encoder.init();

    return ret;
}

Board& get_board()
{
    return board;
}

}  // namespace MM
