/**
* @file nav.h
* @author Bex Saw
* @brief Navigation system for the micromouse using IMU
* @version 0.1
* @date 4/8/2026
*/

#pragma once

#include "bno055_imu.h"
#include "imu_math.h"
// #include "floodfill.h"
// #include "ir_interface.h"

namespace MM
{
class Navigation
{

    enum class State : uint8_t
    {
        STRAIGHT = 0,
        TURNING_LEFT,
        TURNING_RIGHT,
        STOPPED,
        RESET
    };

public:
    Navigation();

    /**
    * @brief Update the navigation system with new IMU data
    * @param imu The latest IMU data from the BNO055 sensor
    */
    void update(const Bno055Data& imu);

    /**
    * @brief Check if a turn is needed based on IMU data
    */
    void check_turn();

    ~Navigation();

private:
    State state;
};
}  // namespace MM