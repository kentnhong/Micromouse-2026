/**
* @file main.cc
* @brief Dual-motor trapezoidal profile + PID — real-time serial debug via USART1
* @author Bex Saw
*/

#include <span>
#include "board.h"
#include "drv8231.h"
#include "enc_math.h"
#include "enc_sample.h"
#include "pid.h"
#include "pid_math.h"
#include "st_timebase.h"
#include "trapezoidal.h"

using namespace MM;

// Physical constants
static constexpr float kPi = 3.14159265358979f;
static constexpr float kWheelDiamMm = 14.0f;
static constexpr float kGearRatio = 15.0f;
static constexpr float kTicksPerRev = 12.0f;
static constexpr float kMmPerTick =
    (kWheelDiamMm * kPi) / (kGearRatio * kTicksPerRev);

/// CHANGE: cruise speed
static constexpr float kCruiseMmPerSec = 150.0f;
static constexpr float kCruiseTicks = kCruiseMmPerSec / kMmPerTick;

/// CHANGE: acceleration / deceleration (ticks/s²)
static constexpr float kAccelTicksPerSec2 = 200.0f;
static constexpr float kDecelTicksPerSec2 = 200.0f;

/// CHANGE: distance in mm (set 0 for ramp-only / no stop)
static constexpr float kCellMm = 360.0f;
static constexpr int32_t kCellTicks =
    static_cast<int32_t>(kCellMm / kMmPerTick);

// USART helper
static void usart_print(Usart& usart, const char* str)
{
    usart.send(std::span<const uint8_t>(reinterpret_cast<const uint8_t*>(str),
                                        __builtin_strlen(str)));
}

static void usart_print_csv(Usart& usart, float t_sec, float speed_mm_s)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%.3f,%.3f\r\n",
             static_cast<double>(t_sec), static_cast<double>(speed_mm_s));
    usart_print(usart, buf);
}

int main()
{
    bsp_init();
    Board& hw = get_board();

    // Timebase (TIM5 @ 1 MHz)
    StTimebaseParams tb_params = {TIM5};
    Stmf4::HwTimebase timebase(tb_params);

    RCC->APB1ENR |= RCC_APB1ENR_TIM5EN;
    timebase.init(50'000'000, 1'000'000, std::chrono::microseconds(0xFFFFFFFF));
    timebase.start();
    Utils::bind_timebase(timebase);

    // PWM frequencies
    hw.pwm1_left.set_frequency(2000);
    hw.pwm2_left.set_frequency(2000);
    hw.pwm1_right.set_frequency(2000);
    hw.pwm2_right.set_frequency(2000);

    //hw.encoder_left.reset_ticks();
    //hw.encoder_right.reset_ticks();

    // PID objects
    /// CHANGE: PID gains
    const Gains kGains{0.001f, 0.0002f, 0.0f};

    PID pid_left(hw.motor_left, kGains);
    PID pid_right(hw.motor_right, kGains);

    pid_left.set_output_limits(-0.30f, 0.30f);
    pid_right.set_output_limits(-0.30f, 0.30f);

    // Trapezoidal profile
    Trapezoidal profile;
    profile.configure(0.0f, kCruiseTicks, kAccelTicksPerSec2,
                      kDecelTicksPerSec2, kCellTicks);

    // Encoder timing (1ms)
    Sample::EncoderTiming timing =
        Sample::init_encoder_timing(hw.encoder_left, hw.encoder_sample_us);

    usart_print(hw.usart, "== PROFILE + PID DEBUG ==\r\n");

    unsigned print_count = 0;

    while (1)
    {

        const int32_t ticks_left =
            Sample::sample_encoder(hw.encoder_left, timing);
        const int32_t ticks_right =
            Sample::sample_encoder(hw.encoder_right, timing);

        usart_print_csv(hw.usart, timing.sample_time_sec, ticks_left);
        usart_print_csv(hw.usart, timing.sample_time_sec, ticks_right);

        profile.trapezoidal(EncoderInput{ticks_left, ticks_right},
                            timing.sample_time_sec);

        // const float target = profile.get_speed_setpoint();
        const float ktarget = 3000.0f;  
        pid_left.update(ktarget, Drv8231::Direction::FORWARD, ticks_left,
                        timing.sample_time_sec);
        pid_right.update(ktarget, Drv8231::Direction::FORWARD, ticks_right,
                         timing.sample_time_sec);

        if (++print_count % 10 != 0)
            continue;

        char hdr[64];
        snprintf(hdr, sizeof(hdr), "--- sp:%.0f t/s  dist:%ld/%ld ticks\r\n",
                 ktarget, static_cast<long>(profile.get_progress_ticks()),
                 static_cast<long>(kCellTicks));
        usart_print(hw.usart, hdr);

    }

    hw.motor_left.drive(Drv8231::Direction::COAST, 0);
    hw.motor_right.drive(Drv8231::Direction::COAST, 0);

    usart_print(hw.usart, "== PROFILE COMPLETE ==\r\n");

    while (1)
    {
    }
}
