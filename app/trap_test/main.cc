/**
 * @file main.cc
 * @brief Trapezoidal profile + PID test — one cell per button press, CSV over USART.
 * @author Bex Saw
 */

#include <array>
#include <cstdio>
#include <span>
#include "board.h"
#include "delay.h"
#include "drv8231.h"
#include "enc_sample.h"
#include "pid.h"
#include "pid_math.h"
#include "trapezoidal.h"

using namespace MM;

static constexpr float kPi               = 3.14159265358979f;
static constexpr float kWheelDiameterMm  = 14.0f;
static constexpr float kGearRatio        = 15.0f;
static constexpr float kTicksPerMotorRev = 8.0f;
static constexpr float kMmPerTick =
    (kWheelDiameterMm * kPi) / (kGearRatio * kTicksPerMotorRev);

static constexpr float   kCellDistanceMm = 125.0f;
static constexpr int32_t kCellTicks = static_cast<int32_t>(kCellDistanceMm / kMmPerTick);

static constexpr uint32_t kEncoderSampleUs = 20'000;

static constexpr float kMinMmPerSec    = 50.0f;
static constexpr float kMaxMmPerSec    = 80.0f;
static constexpr float kAccelMmPerSec2 = 150.0f;
static constexpr float kDecelMmPerSec2 = 150.0f;

static constexpr float kMinTicks   = kMinMmPerSec   / kMmPerTick;
static constexpr float kMaxTicks   = kMaxMmPerSec   / kMmPerTick;
static constexpr float kAccelTicks = kAccelMmPerSec2 / kMmPerTick;
static constexpr float kDecelTicks = kDecelMmPerSec2 / kMmPerTick;

static constexpr Gains kGains{.kp = 0.005f, .ki = 0.0005f, .kd = 0.0f};

static void usart_print(Usart& usart, const char* str)
{
    const uint8_t* data = reinterpret_cast<const uint8_t*>(str);
    uint32_t len = 0;
    while (str[len]) { ++len; }
    usart.send(std::span<const uint8_t>(data, len));
}

int main()
{
    if (!bsp_init())
    {
        return 1;
    }

    Board& hw = get_board();

    PID pid_left{hw.motor_left, kGains};
    PID pid_right{hw.motor_right, kGains};
    pid_left.set_output_limits(-1.0f, 0.80f);
    pid_right.set_output_limits(-1.0f, 0.80f);
    pid_left.set_integral_limit(50.0f);
    pid_right.set_integral_limit(50.0f);

    Sample::EncoderTiming timing =
        Sample::init_encoder_timing(hw.encoder_left, kEncoderSampleUs);

    Trapezoidal profile{};

    char buf[128];

    while (1)
    {
        // Wait for button press (active low with pull-up)
        while (hw.start_bt.read() != 0) {}
        Utils::delay_ms(50);  // debounce
        while (hw.start_bt.read() != 0) {}

        profile.configure(kMinTicks, kMaxTicks, kAccelTicks, kDecelTicks, kCellTicks);
        pid_left.reset();
        pid_right.reset();
        hw.encoder_left.reset_ticks();
        hw.encoder_right.reset_ticks();

        usart_print(hw.usart, "n,goal,prog,setpt,L_ticks,R_ticks\r\n");

        static constexpr float kEmaAlpha = 0.5f;
        float vel_left_smooth  = 0.0f;
        float vel_right_smooth = 0.0f;

        int32_t n = 0;
        while (!profile.is_complete())
        {
            const EncoderInput enc =
                Sample::sample_encoders(hw.encoder_left, hw.encoder_right, timing);
            const float dt = timing.sample_time_sec;

            const float raw_left  = static_cast<float>(-enc.left_ticks);
            const float raw_right = static_cast<float>(-enc.right_ticks);
            vel_left_smooth  = kEmaAlpha * vel_left_smooth  + (1.0f - kEmaAlpha) * raw_left;
            vel_right_smooth = kEmaAlpha * vel_right_smooth + (1.0f - kEmaAlpha) * raw_right;
            const float vel_avg = (vel_left_smooth + vel_right_smooth) * 0.5f;

            profile.trapezoidal(enc, dt);
            const float setpt = profile.get_speed_setpoint();

            const int32_t vel_feedback = static_cast<int32_t>(vel_avg);
            pid_left.update(setpt, Drv8231::Direction::FORWARD, vel_feedback, dt);
            pid_right.update(setpt, Drv8231::Direction::FORWARD, vel_feedback, dt);

            const int32_t prog = profile.get_progress_ticks();
            snprintf(buf, sizeof(buf), "%ld,%ld,%ld,%.1f,%ld,%ld\r\n",
                     static_cast<long>(n),
                     static_cast<long>(kCellTicks),
                     static_cast<long>(prog),
                     static_cast<double>(setpt),
                     static_cast<long>(enc.left_ticks),
                     static_cast<long>(enc.right_ticks));
            usart_print(hw.usart, buf);
            ++n;
        }

        hw.motor_left.drive(Drv8231::Direction::COAST, 0);
        hw.motor_right.drive(Drv8231::Direction::COAST, 0);

        usart_print(hw.usart, "DONE\r\n");

        Utils::delay_ms(500);
    }

    return 0;
}
