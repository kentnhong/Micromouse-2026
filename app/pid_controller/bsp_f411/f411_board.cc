#include <ctime>
#include <tuple>
#include "../board.h"
#include "bno055_imu.h"
#include "drv8231.h"
#include "pid.h"
#include "st_encoder.h"
#include "st_gpio.h"
#include "st_i2c.h"
#include "st_pwm.h"
#include "st_sys_clk.h"

namespace MM
{

static constexpr uint16_t CCR_100KHZ = 0x1F4;
static constexpr uint16_t TRISE_100KHZ = 0x2B;

constexpr Gains kWheelPid{
    250.0f, 5.0f, 0.0f};  // Tuning params depends on what we feed into the PID

constexpr float kTargetSpeedMps = 0.10f;  // 10 cm/s target speed for testing
static constexpr uint32_t kEncoderSampleUs =
    100'000;  // 100ms sample time for encoder readings

/* PWM */

Stmf4::StGpioSettings speed_pwm_output_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 2};  // AF2 for TIM3 on PB4

const Stmf4::StGpioParams left_speed_pwm_output_params{
    4, GPIOB, speed_pwm_output_settings};  // PB4 (TIM3_CH1)
const Stmf4::StGpioParams right_speed_pwm_output_params{
    5, GPIOB, speed_pwm_output_settings};  // PB5 (TIM3_CH2)

Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

const Stmf4::StPwmParams left_speed_pwm_params{TIM3, Stmf4::PwmChannel::CH1,
                                               pwm_settings, 16000000};
const Stmf4::StPwmParams right_speed_pwm_params{TIM3, Stmf4::PwmChannel::CH2,
                                                pwm_settings, 16000000};

Stmf4::StGpioSettings motor_input_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

const Stmf4::StGpioParams left_in1_params{4, GPIOA,
                                          motor_input_settings};  // PA4
const Stmf4::StGpioParams left_in2_params{5, GPIOA,
                                          motor_input_settings};  // PA5
const Stmf4::StGpioParams right_in1_params{0, GPIOB,
                                           motor_input_settings};  // PB0
const Stmf4::StGpioParams right_in2_params{1, GPIOB,
                                           motor_input_settings};  // PB1

/* Encoder */

Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

const Stmf4::StGpioParams left_enc_input_params_1{
    0, GPIOA, enc_input_settings};  // PA0 (TIM2_CH1)
const Stmf4::StGpioParams left_enc_input_params_2{
    1, GPIOA, enc_input_settings};  // PA1 (TIM2_CH2)

Stmf4::StGpioSettings right_enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 2};

const Stmf4::StGpioParams right_enc_input_params_1{
    6, GPIOB, right_enc_input_settings};  // PB6 (TIM4_CH1)
const Stmf4::StGpioParams right_enc_input_params_2{
    7, GPIOB, right_enc_input_settings};  // PB7 (TIM4_CH2)

// Configuration in Encoder
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams left_encoder_params{TIM2, encoder_settings};
const Stmf4::StEncoderParams right_encoder_params{TIM4, encoder_settings};

/* Clock */

Stmf4::HwClk clock(Stmf4::Configuration::HSI_16MHZ);

Stmf4::HwGpio left_in1(left_in1_params);
Stmf4::HwGpio left_in2(left_in2_params);
Stmf4::HwGpio right_in1(right_in1_params);
Stmf4::HwGpio right_in2(right_in2_params);

Stmf4::HwGpio left_speed_pwm_output(left_speed_pwm_output_params);
Stmf4::HwGpio right_speed_pwm_output(right_speed_pwm_output_params);
Stmf4::HwPwm left_speed_pwm(left_speed_pwm_params);
Stmf4::HwPwm right_speed_pwm(right_speed_pwm_params);

Stmf4::HwEncoder left_encoder(left_encoder_params);
Stmf4::HwEncoder right_encoder(right_encoder_params);

Stmf4::HwGpio left_encoder_ch1(left_enc_input_params_1);
Stmf4::HwGpio left_encoder_ch2(left_enc_input_params_2);
Stmf4::HwGpio right_encoder_ch1(right_enc_input_params_1);
Stmf4::HwGpio right_encoder_ch2(right_enc_input_params_2);

Drv8231 left_motor(left_in1, left_in2, left_speed_pwm);
Drv8231 right_motor(right_in1, right_in2, right_speed_pwm);

Board board{.left_encoder = left_encoder,
            .right_encoder = right_encoder,
            .left_speed_pwm = left_speed_pwm,
            .right_speed_pwm = right_speed_pwm,
            .left_motor = left_motor,
            .right_motor = right_motor,
            .left_in1 = left_in1,
            .left_in2 = left_in2,
            .right_in1 = right_in1,
            .right_in2 = right_in2,
            .left_encoder_ch1 = left_encoder_ch1,
            .left_encoder_ch2 = left_encoder_ch2,
            .right_encoder_ch1 = right_encoder_ch1,
            .right_encoder_ch2 = right_encoder_ch2,
            .encoder_sample_us = kEncoderSampleUs};

/* PID */

static PID::PIDConfig config{
    .left = kWheelPid,
    .right = kWheelPid,
};

static PID pid(config);
static PID::MotorOutput output{};
static PID::Target target{kTargetSpeedMps, kTargetSpeedMps};

bool bsp_init()
{
    // Enable GPIOA, GPIOB, TIM2, TIM3, TIM4, and I2C1 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |= RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN |
                    RCC_APB1ENR_TIM4EN | RCC_APB1ENR_I2C1EN;

    clock.init();

    // Initialize GPIOs
    left_in1.init();
    left_in2.init();
    right_in1.init();
    right_in2.init();
    left_speed_pwm_output.init();
    right_speed_pwm_output.init();
    left_encoder_ch1.init();
    left_encoder_ch2.init();
    right_encoder_ch1.init();
    right_encoder_ch2.init();

    // Initialize peripherals
    left_speed_pwm.init();
    right_speed_pwm.init();
    left_encoder.init();
    right_encoder.init();
    left_motor.init();
    right_motor.init();

    return true;
}

Board& get_board()
{
    return board;
}

/* Accessors for PID */

// PID-related objects
std::tuple<PID&, PID::MotorOutput&, PID::Target&> get_pid_bundle()
{
    return std::tie(pid, output, target);
}
}  // namespace MM
