/**
* @file test.cc
* @brief PID controller test application
* @author Bex Saw
*/

#include <gtest/gtest.h>
#include "drv8231.h"
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

TEST(PIDTest, ConvertsTicksToTicksPerSecond)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    float ticks_per_sec = 0.0f;

    EXPECT_TRUE(pid.ticks_to_ticks_per_second(25, 0.01f, ticks_per_sec));
    EXPECT_FLOAT_EQ(ticks_per_sec, 2500.0f);
}

TEST(PIDTest, RejectsZeroOrNegativeDt)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    float ticks_per_sec = 123.0f;

    EXPECT_FALSE(pid.ticks_to_ticks_per_second(25, 0.0f, ticks_per_sec));
    EXPECT_FLOAT_EQ(ticks_per_sec, 0.0f);
}

TEST(PIDTest, DrivesForwardWhenOutputIsPositive)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.01f, 0.0f, 0.0f});

    pid.update(100.0f, Drv8231::Direction::FORWARD, 0, 1.0f);

    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));
}

TEST(PIDTest, ReversesDirectionWhenOutputIsNegative)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.01f, 0.0f, 0.0f});

    pid.update(0.0f, Drv8231::Direction::FORWARD, 100, 1.0f);

    EXPECT_FALSE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::REVERSE));
}

TEST(PIDTest, LimitsDutyCycleTo100Percent)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    pid.update(1000.0f, Drv8231::Direction::FORWARD, 0, 1.0f);

    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));
}