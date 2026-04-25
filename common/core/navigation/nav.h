/**
* @file nav.h
* @author Bex Saw
* @brief Navigation system for the micromouse using IMU
* @version 0.1
* @date 4/8/2026
*/

#pragma once 

#include "imu_math.h"
#include "bno055.h"

/**
* @note Use IMU yaw to decide when to start/stop a turn, 
* or to correct the robot’s heading during straight runs
*/

namespace MM
{
class Nav
{
public:

    // Constructor
    Nav();

    // Having a function where it holds the statemachine for the navigation control

    // Function that checking the imu data and decide when the start/stop a turn (FOR OVERHEADING)

    ~Nav();

private:

};
}