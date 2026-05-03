/**
* @file test.cc
* @brief PID controller test application
* @author Bex Saw
*/

#include <gtest/gtest.h>
#include <iostream>
#include <string>
#include "drv8231.h"
#include "enc_sample.h"
#include "pid.h"

using namespace MM;

class FakeGpio : public Gpio
{
public:
    bool value = false;

    bool toggle() override
    {
        value = !value;
        return true;
    }

    bool set(const bool active) override
    {
        value = active;
        return true;
    }

    bool read() override
    {
        return value;
    }
};

class FakePwm : public Pwm
{
public:
    uint8_t duty = 0;

    bool set_duty_cycle(uint8_t duty_cycle) override
    {
        duty = duty_cycle;
        return true;
    }
};

class FakeEncoder : public Encoder
{
public:
    int32_t ticks = 0;
    uint32_t time_cycles = 0;
    uint32_t cycles_per_microsecond = 16;

    int32_t get_ticks() override
    {
        return ticks;
    }

    bool reset_ticks() override
    {
        ticks = 0;
        return true;
    }

    bool init_cycle_counter() override
    {
        time_cycles = 0;
        return true;
    }

    uint32_t get_time_cycles() override
    {
        return time_cycles;
    }

    uint32_t cycles_per_us() override
    {
        return cycles_per_microsecond;
    }

    EncoderStats measure_encoder_stats(uint32_t sample_cycles,
                                       float ticks_per_output_rev,
                                       float cm_per_tick) override
    {
        (void)sample_cycles;
        (void)ticks_per_output_rev;
        (void)cm_per_tick;
        return {};
    }
};

static const char* motor_state_name(int state)
{
    switch (static_cast<Drv8231::Direction>(state))
    {
        case Drv8231::Direction::COAST:
            return "COAST";
        case Drv8231::Direction::FORWARD:
            return "FORWARD";
        case Drv8231::Direction::REVERSE:
            return "REVERSE";
        case Drv8231::Direction::BRAKE:
            return "BRAKE";
    }

    return "UNKNOWN";
}

static void print_motor_output(const std::string& label, const FakeGpio& in1,
                               const FakeGpio& in2, const FakePwm& pwm,
                               const Drv8231& motor)
{
    std::cout << label << ": in1=" << std::boolalpha << in1.value
              << " in2=" << in2.value << " duty=" << static_cast<int>(pwm.duty)
              << " state=" << motor_state_name(motor.get_state()) << '\n';
}

///*****************************///
///         TEST CASES          ///
///*****************************///

TEST(EncoderSampleTest, 1_OneMillisecondSampleCreatesOneKilohertzTiming)
{
    FakeEncoder encoder;
    encoder.cycles_per_microsecond = 16;

    const Sample::EncoderTiming timing =
        Sample::init_encoder_timing(encoder, 1'000);

    EXPECT_EQ(timing.sample_cycles, 16'000u);
    EXPECT_FLOAT_EQ(timing.sample_time_sec, 0.001f);
}

TEST(PIDTest, 1_ConvertsTicksToTicksPerSecond)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float ticks_per_sec = 0.0f;

    EXPECT_TRUE(pid.ticks_to_ticks_per_second(25, 0.01f, ticks_per_sec));
    std::cout << "ticks_to_ticks_per_second: ticks=25 dt=0.01 output="
              << ticks_per_sec << '\n';
    EXPECT_TRUE(pid.ticks_to_ticks_per_second(-50, 0.02f, ticks_per_sec));
    std::cout << "ticks_to_ticks_per_second: ticks=-50 dt=0.02 output="
              << ticks_per_sec << '\n';

    // Expect GTEST result: ticks_to_ticks_per_second: ticks=25 dt=0.01 output=2500
    // Expect GTEST result: ticks_to_ticks_per_second: ticks=-50 dt=0.02 output=-2500
}

TEST(PIDTest, 2_RejectsZeroOrNegativeDt)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float ticks_per_sec = 0.0f;

    EXPECT_FALSE(pid.ticks_to_ticks_per_second(25, 0.0f, ticks_per_sec));
    std::cout << "ticks_to_ticks_per_second: ticks=25 dt=0.0 output="
              << ticks_per_sec << '\n';
    EXPECT_FALSE(pid.ticks_to_ticks_per_second(25, -0.01f, ticks_per_sec));
    std::cout << "ticks_to_ticks_per_second: ticks=25 dt=-0.01 output="
              << ticks_per_sec << '\n';

    // Expect GTEST result: ticks_to_ticks_per_second: ticks=25 dt=0.0 output=0
    // Expect GTEST result: ticks_to_ticks_per_second: ticks=25 dt=-0.01 output=0
}

TEST(PIDTest, 3_DrivesForwardWhenOutputIsPositive)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float output = 0.5f;  // 50% forward
    EXPECT_TRUE(pid.update(output, Drv8231::Direction::FORWARD, 0, 0.01f));
    print_motor_output("DrivesForwardWhenOutputIsPositive", in1, in2, pwm,
                       motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 50);

    // Expect GTEST result: DrivesForwardWhenOutputIsPositive: in1=true in2=false duty=50 state=FORWARD
}

TEST(PIDTest, 4_ReversesDirectionWhenOutputIsNegative)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float output = -0.75f;  // 75% reverse
    EXPECT_TRUE(pid.update(output, Drv8231::Direction::FORWARD, 0, 0.01f));
    print_motor_output("ReversesDirectionWhenOutputIsNegative", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 75);

    // Expect GTEST result: ReversesDirectionWhenOutputIsNegative: in1=false in2=true duty=75 state=REVERSE
}

TEST(PIDTest, 5_LimitsDutyCycleTo100Percent)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float output = 1.5f;  // 150% forward, should be limited to 100%
    EXPECT_TRUE(pid.update(output, Drv8231::Direction::FORWARD, 0, 0.01f));
    print_motor_output("LimitedDutyCycleTo100Percent", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 100);

    output = -2.0f;  // 200% reverse, should be limited to 100%
    EXPECT_TRUE(pid.update(output, Drv8231::Direction::FORWARD, 0, 0.01f));
    print_motor_output("LimitedDutyCycleTo100Percent", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 100);

    // Expect GTEST result: LimitedDutyCycleTo100Percent: in1=true in2=false duty=100 state=FORWARD
    // Expect GTEST result: LimitedDutyCycleTo100Percent: in1=false in2=true duty=100 state=REVERSE
}

TEST(PIDTest, 6_CoastsWhenOutputIsZero)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float output = 0.0f;  // Should coast
    EXPECT_TRUE(pid.update(output, Drv8231::Direction::FORWARD, 0, 0.01f));
    print_motor_output("CoastsWhenOutputIsZero", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);

    // Expect GTEST result: CoastsWhenOutputIsZero: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest, 7_ReversesWhenDesiredSpeedIsNegative)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = -100.0f;  // Negative setpoint, should reverse
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 0;
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ReversesWhenDesiredSpeedIsNegative", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty,
              100);  // Should be full reverse to reach negative setpoint

    // Expect GTEST result: ReversesWhenDesiredSpeedIsNegative: in1=false in2=true duty=100 state=REVERSE
}

TEST(PIDTest, 8_DrivesReverseWhenPolarityIsReverseAndOutputIsPositive)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    float output = 0.5f;  // 50% forward output
    Drv8231::Direction polarity =
        Drv8231::Direction::REVERSE;  // Desired reverse

    EXPECT_TRUE(pid.update(output, polarity, 0, 0.01f));
    print_motor_output("DrivesReverseWhenPolarityIsReverseAndOutputIsPositive",
                       in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 50);  // Should drive reverse at 50% duty

    // Expect GTEST result: DrivesReverseWhenPolarityIsReverseAndOutputIsPositive: in1=false in2=true duty=50 state=REVERSE
}

TEST(PIDTest, 9_ReversesToSlowDownWhenZeroSetpointAndMovingForward)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = 0.0f;  // Zero setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 100;     // Currently moving forward
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ReversesToSlowDownWhenZeroSetpointAndMovingForward",
                       in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty,
              100);  // Should reverse at full duty to slow down to zero

    // Expect GTEST result: ReversesToSlowDownWhenZeroSetpointAndMovingForward: in1=false in2=true duty=100 state=REVERSE
}

TEST(PIDTest, 10_CompensateForErrorGains)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.05f, 0.01f));  // Non-zero gains

    float desired_speed_ticks = 5000.0f;  // Setpoint in ticks/s
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 25;      // 2500 ticks/s, half the setpoint
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("CompensateForErrorGains", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(pwm.duty,
              0);  // Should drive forward with some duty to correct error

    // Expect GTEST result: CompensateForErrorGains: in1=true in2=false duty=some_value state=FORWARD
}

TEST(PIDTest, 11_CoastsWhenMeasuredSpeedMatchesSetpoint)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 1;       // 100 ticks/s, matching the setpoint
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("CoastsWhenMeasuredSpeedMatchesSetpoint", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);  // Should coast with zero duty

    // Expect GTEST result: CoastsWhenMeasuredSpeedMatchesSetpoint: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest, 12_UsesReversePolarityForForwardCorrectionWhenRequestedReverse)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = 5000.0f;  // Setpoint in ticks/s
    Drv8231::Direction polarity = Drv8231::Direction::REVERSE;
    int32_t measured_ticks = 25;  // 2500 ticks/s, half the setpoint
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output(
        "UsesReversePolarityForForwardCorrectionWhenRequestedReverse", in1, in2,
        pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_GT(
        pwm.duty,
        0);  // Should drive reverse to correct forward error when requested reverse

    // Expect GTEST result: UsesReversePolarityForForwardCorrectionWhenRequestedReverse: in1=false in2=true duty=some_value state=REVERSE
}

TEST(PIDTest, 13_FlipsToForwardWhenReversePolarityOutputIsNegative)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = -50.0f;  // Negative setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::REVERSE;  // Desired reverse
    int32_t measured_ticks = 25;      // Currently at half the setpoint
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("FlipsToForwardWhenReversePolarityOutputIsNegative", in1,
                       in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(
        pwm.duty,
        0);  // Should drive forward to correct reverse error when requested reverse

    // Expect GTEST result: FlipsToForwardWhenReversePolarityOutputIsNegative: in1=true in2=false duty=some_value state=FORWARD
}

TEST(PIDTest, 14_UsesMeasuredTicksOverDtAsMeasuredSpeed)
{

    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    float desired_speed_ticks = 10000.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 50;      // Measured ticks
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("UsesMeasuredTicksOverDtAsMeasuredSpeed", in1, in2, pwm,
                       motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(
        pwm.duty,
        0);  // Should drive forward to correct error based on measured speed of 5000 ticks/sec

    // Expect GTEST result: UsesMeasuredTicksOverDtAsMeasuredSpeed: in1=true in2=false duty=some_value state=FORWARD
}

TEST(PIDTest, 15_IntegralTermIncreasesDutyAcrossUpdates)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 0.1f, 0.0f));  // Only integral gain

    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 0;       // Initially at zero speed
    float dt_sec = 0.01f;

    // First update should have some duty due to integral term starting to accumulate error
    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("IntegralTermIncreasesDutyAcrossUpdates - Update 1", in1,
                       in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(pwm.duty, 0);

    // Simulate some time passing with no change in measured ticks to allow integral to accumulate more
    for (int i = 0; i < 5; ++i)
    {
        EXPECT_TRUE(
            pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
        print_motor_output("IntegralTermIncreasesDutyAcrossUpdates - Update " +
                               std::to_string(i + 2),
                           in1, in2, pwm, motor);
        EXPECT_TRUE(in1.value);
        EXPECT_FALSE(in2.value);
        EXPECT_GT(
            pwm.duty,
            0);  // Duty should increase as integral accumulates more error
    }

    // Expect GTEST result: IntegralTermIncreasesDutyAcrossUpdates - Update 1: in1=true in2=false duty=some_value state=FORWARD
}

TEST(PIDTest, 16_ResetClearsIntegralAccumulation)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 0.1f, 0.0f));  // Only integral gain

    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 0;       // Initially at zero speed
    float dt_sec = 0.01f;
    // First update to capture the duty from one sample of integral error.
    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ResetClearsIntegralAccumulation - First Update", in1,
                       in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(pwm.duty, 0);
    uint8_t first_update_duty = pwm.duty;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ResetClearsIntegralAccumulation - Before Reset", in1,
                       in2, pwm, motor);
    EXPECT_GT(pwm.duty, first_update_duty);

    // Now reset and expect duty to return to the first-update value.
    EXPECT_TRUE(pid.reset());
    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ResetClearsIntegralAccumulation - After Reset", in1,
                       in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, first_update_duty);

    // Expect GTEST result: ResetClearsIntegralAccumulation - Before Reset: in1=true in2=false duty=increased state=FORWARD
    // Expect GTEST result: ResetClearsIntegralAccumulation - After Reset: in1=true in2=false duty=first-update state=FORWARD
}

TEST(PIDTest, 17_OutputLimitsClampDutyCycle)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));

    // Set output limits to -0.5 to 0.5
    EXPECT_TRUE(pid.set_output_limits(-0.5f, 0.5f));

    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 0;       // Initially at zero speed
    float dt_sec = 0.01f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("OutputLimitsClampDutyCycle", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty,
              50);  // Should be limited to 50% duty due to output limits

    // Expect GTEST result: OutputLimitsClampDutyCycle: in1=true in2=false duty=50 state=FORWARD
    // Note: If the output limits were not applied, we would expect a duty of 100% here, so this test verifies that the limits are correctly clamping the output.
}

TEST(PIDTest, 18_IntegralLimitClampsAccumulatedError)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 0.1f, 0.0f));  // Only integral gain

    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity =
        Drv8231::Direction::FORWARD;  // Desired forward
    int32_t measured_ticks = 0;       // Initially at zero speed
    float dt_sec = 0.01f;

    // Simulate many updates to try to accumulate a large integral error
    for (int i = 0; i < 200; ++i)
    {
        EXPECT_TRUE(
            pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
        print_motor_output("IntegralLimitClampsAccumulatedError - Update " +
                               std::to_string(i + 1),
                           in1, in2, pwm, motor);
        EXPECT_TRUE(in1.value);
        EXPECT_FALSE(in2.value);
        EXPECT_LE(pwm.duty,
                  100);  // Duty should never exceed 100% due to integral limit
    }

    // Expect GTEST result: IntegralLimitClampsAccumulatedError - Update 1: in1=true in2=false duty=some_value state=FORWARD
    // Expect GTEST result: IntegralLimitClampsAccumulatedError - Update 200: in1=true in2=false duty=100 state=FORWARD
    // Note: This test verifies that even after many updates with a large error, the integral term does not cause the duty cycle to exceed 100%, which indicates that the integral limit is correctly clamping the accumulated error.
}

TEST(PIDTest, 19_RejectsZeroDtInUpdate)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));
    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity = Drv8231::Direction::FORWARD;
    int32_t measured_ticks = 0;  // Initially at zero speed
    float dt_sec = 0.0f;         // Invalid zero dt

    EXPECT_FALSE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("RejectsZeroDtInUpdate", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);  // Should not update motor output with invalid dt

    // Expect GTEST result: RejectsZeroDtInUpdate: in1=false in2=false duty=0 state=COAST
    // Note: This test verifies that if the update method is called with a zero dt, it should reject the update and not change the motor output, which is important to prevent division by zero and ensure stable behavior.
}

TEST(PIDTest, 20_RejectsNegativeDtInUpdate)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.1f, 0.0f, 0.0f));
    float desired_speed_ticks = 100.0f;  // Setpoint
    Drv8231::Direction polarity = Drv8231::Direction::FORWARD;
    int32_t measured_ticks = 0;  // Initially at zero speed
    float dt_sec = -0.01f;       // Invalid negative dt

    EXPECT_FALSE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("RejectsNegativeDtInUpdate", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);  // Should not update motor output with invalid dt

    // Expect GTEST result: RejectsNegativeDtInUpdate: in1=false in2=false duty=0 state=COAST
    // Note: This test verifies that if the update method is called with a negative dt, it should reject the update and not change the motor output, which is important to prevent division by zero and ensure stable behavior.
}

TEST(PIDTest_Specific, 1_ExpectPIDMathTest)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 drv(in1, in2, pwm);
    PID pid(drv, Gains(0.25f, 0.125f, 0.125f));

    float desired_speed_ticks = 1.0f;
    Drv8231::Direction polarity = Drv8231::Direction::FORWARD;
    int32_t measured_ticks = 0;
    float dt_sec = 1.0f;

    EXPECT_TRUE(
        pid.update(desired_speed_ticks, polarity, measured_ticks, dt_sec));
    print_motor_output("ExpectPIDMathTest", in1, in2, pwm, drv);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 50);
    EXPECT_EQ(drv.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));

    // Expected GTEST result: ExpectPIDMathTest: in1=true in2=false duty=40 state=FORWARD
}

TEST(PIDTest_Specific, 2_DerivativeBehavior)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 0.0f, 0.1f));

    EXPECT_TRUE(pid.update(0.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    EXPECT_EQ(pwm.duty, 0);

    EXPECT_TRUE(pid.update(5.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("DerivativeBehavior - rising error", in1, in2, pwm,
                       motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 50);

    EXPECT_TRUE(pid.update(5.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("DerivativeBehavior - steady error", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);

    // Simulate a sudden drop in error to trigger negative derivative
    EXPECT_TRUE(pid.update(0.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("DerivativeBehavior - falling error", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 50);

    // Expected GTEST results:
    // DerivativeBehavior - rising error: in1=true in2=false duty=50 state=FORWARD
    // DerivativeBehavior - steady error: in1=false in2=false duty=0 state=COAST
    // DerivativeBehavior - falling error: in1=false in2=true duty=50 state=REVERSE
}

TEST(PIDTest_Specific, 3_IntegralBehavior)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 1.0f, 0.0f));
    EXPECT_TRUE(pid.set_integral_limit(0.4f));

    for (int i = 0; i < 5; ++i)
    {
        EXPECT_TRUE(pid.update(10.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
        print_motor_output("IntegralBehavior - Update " + std::to_string(i + 1),
                           in1, in2, pwm, motor);
        EXPECT_TRUE(in1.value);
        EXPECT_FALSE(in2.value);
        EXPECT_EQ(pwm.duty, 40);
    }

    // Expected GTEST results:
    // IntegralBehavior - Update 1: in1=true in2=false duty=10 state
    // IntegralBehavior - Update 2: in1=true in2=false duty=20 state=FORWARD
    // IntegralBehavior - Update 3: in1=true in2=false duty=30 state=FORWARD
}

TEST(PIDTest_Specific, 4_OutputLimitValidation)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));
    EXPECT_TRUE(pid.set_output_limits(0.25f, -0.25f));

    EXPECT_TRUE(pid.update(10.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("OutputLimitValidation - forward", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 25);

    EXPECT_TRUE(pid.update(-10.0f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("OutputLimitValidation - reverse", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 25);

    // Expected GTEST results:
    // OutputLimitValidation - forward: in1=true in2=false duty=25 state=FORWARD
    // OutputLimitValidation - reverse: in1=false in2=true duty=25 state=REVERSE
}

TEST(PIDTest_Specific, 5_ZeroGains)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(10000.0f, Drv8231::Direction::FORWARD, -100, 0.01f));
    print_motor_output("ZeroGains", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));

    // Expected GTEST result: ZeroGains: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest_Specific, 6_ReverseMeasuredTicks)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.01f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.0f, Drv8231::Direction::FORWARD, -50, 1.0f));
    print_motor_output("ReverseMeasuredTicks", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 50);

    // Expected GTEST result: ReverseMeasuredTicks: in1=true in2=false duty=50 state=FORWARD
}

TEST(PIDTest_Specific, 7_SmallOutputRounding)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.009f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output("SmallOutputRounding", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));

    // Expected GTEST result: SmallOutputRounding: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest_Specific, 8_InvalidPolarity)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_FALSE(pid.update(1.0f, Drv8231::Direction::COAST, 0, 1.0f));
    print_motor_output("InvalidPolarity", in1, in2, pwm, motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));

    // Expected GTEST result: InvalidPolarity: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest_Specific, 9_LargeDtHandling)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.5f, Drv8231::Direction::FORWARD, 0, 10.0f));
    print_motor_output("LargeDtHandling", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 50);

    // Expected GTEST result: LargeDtHandling: in1=true in2=false duty=50 state=FORWARD
}

TEST(PIDTest_Specific, 10_MotorCommandStateAfterFailedUpdate)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.8f, Drv8231::Direction::FORWARD, 0, 1.0f));
    EXPECT_EQ(pwm.duty, 80);

    EXPECT_FALSE(pid.update(0.8f, Drv8231::Direction::FORWARD, 0, 0.0f));
    print_motor_output("MotorCommandStateAfterFailedUpdate", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));

    // Expected GTEST result: MotorCommandStateAfterFailedUpdate: in1=false in2=false duty=0 state=COAST
}

TEST(PIDTest_Specific, 11_MotorCommandStateAfterReset)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.6f, Drv8231::Direction::FORWARD, 0, 1.0f));
    EXPECT_TRUE(pid.reset());
    print_motor_output("MotorCommandStateAfterReset", in1, in2, pwm, motor);
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 60);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));

    //  Expected GTEST result: MotorCommandStateAfterReset: in1=true in2=false duty=60 state=FORWARD
}

TEST(PIDTest_Specific, 12_MotorCommandStateAfterSettingOutputLimits)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(1.0f, 0.0f, 0.0f));

    EXPECT_TRUE(pid.update(0.8f, Drv8231::Direction::FORWARD, 0, 1.0f));
    EXPECT_TRUE(pid.set_output_limits(-0.2f, 0.2f));
    print_motor_output(
        "MotorCommandStateAfterSettingOutputLimits - before update", in1, in2,
        pwm, motor);
    EXPECT_EQ(pwm.duty, 80);

    EXPECT_TRUE(pid.update(0.8f, Drv8231::Direction::FORWARD, 0, 1.0f));
    print_motor_output(
        "MotorCommandStateAfterSettingOutputLimits - after update", in1, in2,
        pwm, motor);
    EXPECT_EQ(pwm.duty, 20);

    // Expected GTEST results:
    // MotorCommandStateAfterSettingOutputLimits - before update: in1=true in2=false duty=80 state=FORWARD
    // MotorCommandStateAfterSettingOutputLimits - after update: in1=true in2=false duty=20 state=FORWARD
}

TEST(PIDTest_Specific, 13_RealisticSequence)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains(0.01f, 0.0f, 0.0f));
    const float desired_speed_ticks = 100.0f;
    const float dt_sec = 1.0f;

    EXPECT_TRUE(pid.update(desired_speed_ticks, Drv8231::Direction::FORWARD, 0,
                           dt_sec));
    print_motor_output("RealisticSequence - stopped", in1, in2, pwm, motor);
    EXPECT_EQ(pwm.duty, 100);

    EXPECT_TRUE(pid.update(desired_speed_ticks, Drv8231::Direction::FORWARD, 25,
                           dt_sec));
    print_motor_output("RealisticSequence - quarter speed", in1, in2, pwm,
                       motor);
    EXPECT_EQ(pwm.duty, 75);

    EXPECT_TRUE(pid.update(desired_speed_ticks, Drv8231::Direction::FORWARD, 50,
                           dt_sec));
    print_motor_output("RealisticSequence - half speed", in1, in2, pwm, motor);
    EXPECT_EQ(pwm.duty, 50);

    EXPECT_TRUE(pid.update(desired_speed_ticks, Drv8231::Direction::FORWARD, 75,
                           dt_sec));
    print_motor_output("RealisticSequence - three quarter speed", in1, in2, pwm,
                       motor);
    EXPECT_EQ(pwm.duty, 25);

    EXPECT_TRUE(pid.update(desired_speed_ticks, Drv8231::Direction::FORWARD,
                           100, dt_sec));
    print_motor_output("RealisticSequence - target speed", in1, in2, pwm,
                       motor);
    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);

    // Expected GTEST results:
    // RealisticSequence - stopped: in1=true in2=false duty=100 state=FORWARD
    // RealisticSequence - quarter speed: in1=true in2=false duty=75 state=FORWARD
    // RealisticSequence - half speed: in1=true in2=false duty=50 state=FORWARD
    // RealisticSequence - three quarter speed: in1=true in2=false duty=25 state=FORWARD
    // RealisticSequence - target speed: in1=false in2=false duty=0 state
}
