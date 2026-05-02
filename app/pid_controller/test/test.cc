/**
* @file test.cc
* @brief PID controller test application
* @author Bex Saw
*/

#include <gtest/gtest.h>
#include <iostream>
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

static void print_motor_output(const char* label, const FakeGpio& in1, const FakeGpio& in2,
                               const FakePwm& pwm, const Drv8231& motor)
{
    std::cout << label << ": in1=" << std::boolalpha << in1.value
              << " in2=" << in2.value
              << " duty=" << static_cast<int>(pwm.duty)
              << " state=" << motor_state_name(motor.get_state()) << '\n';
}

TEST(PIDTest, ConvertsTicksToTicksPerSecond)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    float ticks_per_sec = 0.0f;

    EXPECT_TRUE(pid.ticks_to_ticks_per_second(25, 0.01f, ticks_per_sec));
    std::cout << "ticks_to_ticks_per_second: ticks=25 dt=0.01 output="
              << ticks_per_sec << '\n';
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
    std::cout << "ticks_to_ticks_per_second: ticks=25 dt=0.0 output="
              << ticks_per_sec << '\n';
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
    print_motor_output("forward output", in1, in2, pwm, motor);

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
    print_motor_output("reverse output", in1, in2, pwm, motor);

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
    print_motor_output("limited output", in1, in2, pwm, motor);

    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));
}

TEST(PIDTest, CoastsWhenOutputIsZero)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    pid.update(0.0f, Drv8231::Direction::FORWARD, 0, 1.0f);
    print_motor_output("zero output", in1, in2, pwm, motor);

    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));
}

TEST(PIDTest, BrakesWhenOutputIsNegativeButDirectionIsForward)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    pid.update(-50.0f, Drv8231::Direction::FORWARD, 0, 1.0f);
    print_motor_output("negative forward command", in1, in2, pwm, motor);

    EXPECT_TRUE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 50);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::BRAKE));
}

TEST(PIDTest, BrakesWhenOutputIsPositiveButDirectionIsReverse)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    pid.update(50.0f, Drv8231::Direction::REVERSE, 0, 1.0f);
    print_motor_output("positive reverse command", in1, in2, pwm, motor);

    EXPECT_TRUE(in1.value);
    EXPECT_TRUE(in2.value);
    EXPECT_EQ(pwm.duty, 50);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::BRAKE));
}

TEST(PIDTest, HandlesZeroSetpoint)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    pid.update(0.0f, Drv8231::Direction::FORWARD, 100, 1.0f);
    print_motor_output("zero setpoint", in1, in2, pwm, motor);

    EXPECT_FALSE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_EQ(pwm.duty, 0);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::COAST));
}

TEST(PIDTest, HandleNoiseInMeasurement)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.0f, 0.0f});

    // Simulate noise by alternating between slightly above and below the setpoint
    for (int i = 0; i < 10; ++i)
    {
        float measurement = (i % 2 == 0) ? 99.0f : 101.0f;  // Oscillate around setpoint of 100
        pid.update(measurement, Drv8231::Direction::FORWARD, 100, 1.0f);
        print_motor_output("noise update", in1, in2, pwm, motor);
    }

    // Expect the controller to still drive forward with some duty cycle
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(pwm.duty, 0);
    EXPECT_LT(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));
}

TEST(PIDTest, CompensateForErrorGains)
{
    FakeGpio in1;
    FakeGpio in2;
    FakePwm pwm;

    Drv8231 motor(in1, in2, pwm);
    PID pid(motor, Gains{0.1f, 0.05f, 0.01f});

    // Simulate a scenario where the measurement is consistently below the setpoint
    for (int i = 0; i < 10; ++i)
    {
        pid.update(90.0f, Drv8231::Direction::FORWARD, 100, 1.0f);
        print_motor_output("gain compensation update", in1, in2, pwm, motor);
    }

    // Expect the controller to increase duty cycle over time to compensate for error
    EXPECT_TRUE(in1.value);
    EXPECT_FALSE(in2.value);
    EXPECT_GT(pwm.duty, 0);
    EXPECT_LT(pwm.duty, 100);
    EXPECT_EQ(motor.get_state(), static_cast<int>(Drv8231::Direction::FORWARD));
}
