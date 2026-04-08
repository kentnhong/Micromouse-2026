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
constexpr Val kWheelPid{250.0f, 5.0f, 0.0f};
constexpr float kTargetSpeedMps = 0.10f;

/* PWM */

Stmf4::StGpioSettings pwm_output_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::NO_PULL, 2};  // AF2 for TIM3 on PB4

const Stmf4::StGpioParams pwm_output_params{4, GPIOB, pwm_output_settings};

Stmf4::StPwmSettings pwm_settings{Stmf4::PwmMode::EDGE_ALIGNED,
                                  Stmf4::PwmOutputMode::PWM_MODE_1,
                                  Stmf4::PwmDir::UPCOUNTING};

const Stmf4::StPwmParams pwm_params{TIM3, Stmf4::PwmChannel::CH1, pwm_settings,
                                    16000000};

Stmf4::StGpioSettings in1_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

const Stmf4::StGpioParams in1_params{0, GPIOA, in1_settings};  // PA0

Stmf4::StGpioSettings in2_settings{
    Stmf4::GpioMode::GPOUT, Stmf4::GpioOtype::PUSH_PULL,
    Stmf4::GpioOspeed::HIGH, Stmf4::GpioPupd::NO_PULL, 0};

const Stmf4::StGpioParams in2_params{1, GPIOA, in2_settings};  // PA1

/* Encoder */

Stmf4::StGpioSettings enc_input_settings{
    Stmf4::GpioMode::AF, Stmf4::GpioOtype::PUSH_PULL, Stmf4::GpioOspeed::LOW,
    Stmf4::GpioPupd::PULL_UP, 1};

const Stmf4::StGpioParams enc_input_params_1{
    0, GPIOA, enc_input_settings};  // PA0 (TIM2_CH1)
const Stmf4::StGpioParams enc_input_params_2{
    1, GPIOA, enc_input_settings};  // PA1 (TIM2_CH2)

// Configuration in Encoder
Stmf4::StEncoderSettings encoder_settings{
    Stmf4::EncMode::MODE_3, Stmf4::EncChannel::BOTH,
    Stmf4::EncInputPolarity::RISING, Stmf4::EncSlaveMode::DISABLED};

const Stmf4::StEncoderParams encoder_params{TIM2, encoder_settings};

/* Clock */

Stmf4::HwClk clock(Stmf4::Configuration::HSI_16MHZ);

Stmf4::HwGpio in1(in1_params);
Stmf4::HwGpio in2(in2_params);

Stmf4::HwGpio pwm_output(pwm_output_params);
Stmf4::HwPwm pwm(pwm_params);

Stmf4::HwEncoder encoder(encoder_params);
Stmf4::HwGpio encoder_ch1(enc_input_params_1);
Stmf4::HwGpio encoder_ch2(enc_input_params_2);

Drv8231 drv8231(in1, in2, pwm);

Board board{.encoder = encoder,
            .pwm = pwm,
            .drv8231 = drv8231,
            .in1 = in1,
            .in2 = in2};

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
    // Enable GPIOA, GPIOB, TIM2, TIM3, and I2C1 clocks
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;
    RCC->APB1ENR |=
        RCC_APB1ENR_TIM2EN | RCC_APB1ENR_TIM3EN | RCC_APB1ENR_I2C1EN;

    clock.init();

    // Initialize GPIOs
    in1.init();
    in2.init();
    pwm_output.init();
    encoder_ch1.init();
    encoder_ch2.init();

    // Initialize peripherals
    pwm.init();
    encoder.init();
    drv8231.init();

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