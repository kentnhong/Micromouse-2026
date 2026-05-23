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

static void print_motor(Usart& usart, const char* label, const PidDebug& d,
                        int32_t ticks)
{
    const float abs_out = d.output >= 0.0f ? d.output : -d.output;
    const int duty = static_cast<int>(abs_out * 100.0f);
    const char* dir = (d.output > 0.01f)    ? "FWD"
                      : (d.output < -0.01f) ? "BRK"
                                            : "COAST";

    char buf[160];
    snprintf(buf, sizeof(buf),
             "%s\r\n"
             "  ticks  : %ld\r\n"
             "  error  : %.2f\r\n"
             "  P      : %.4f\r\n"
             "  I      : %.4f\r\n"
             "  D      : %.4f\r\n"
             "  out    : %.3f\r\n"
             "  status : %s %d%%\r\n",
             label, static_cast<long>(ticks), d.error, d.p, d.i_term, d.d,
             d.output, dir, duty);

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

    hw.encoder_left.reset_ticks();
    hw.encoder_right.reset_ticks();

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

    // Encoder timing (20 ms sample)
    Sample::EncoderTiming timing =
        Sample::init_encoder_timing(hw.encoder_left, hw.encoder_sample_us);

    usart_print(hw.usart, "== PROFILE + PID DEBUG ==\r\n");

    unsigned print_count = 0;

    while (!profile.is_complete())
    {
        const int32_t ticks_left =
            Sample::sample_encoder(hw.encoder_left, timing);
        const int32_t ticks_right =
            Sample::sample_encoder(hw.encoder_right, timing);

        profile.trapezoidal(EncoderInput{ticks_left, ticks_right},
                            timing.sample_time_sec);

        const float target = profile.get_speed_setpoint();

        pid_left.update(target, Drv8231::Direction::FORWARD, ticks_left,
                        timing.sample_time_sec);
        pid_right.update(target, Drv8231::Direction::FORWARD, ticks_right,
                         timing.sample_time_sec);

        if (++print_count % 10 != 0)
            continue;

        char hdr[64];
        snprintf(hdr, sizeof(hdr), "--- sp:%.0f t/s  dist:%ld/%ld ticks\r\n",
                 target, static_cast<long>(profile.get_progress_ticks()),
                 static_cast<long>(kCellTicks));
        usart_print(hw.usart, hdr);

        print_motor(hw.usart, "LEFT:", pid_left.get_debug(), ticks_left);
        print_motor(hw.usart, "RIGHT:", pid_right.get_debug(), ticks_right);
    }

    hw.motor_left.drive(Drv8231::Direction::COAST, 0);
    hw.motor_right.drive(Drv8231::Direction::COAST, 0);

    usart_print(hw.usart, "== PROFILE COMPLETE ==\r\n");

    while (1)
    {
    }
}
