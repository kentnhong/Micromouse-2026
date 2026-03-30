/**
 * @file imu_math.h
 * @brief Common math types for IMU and sensor data
 * @author Bex Saw
 * @date 2025-12-09
 */

#pragma once

namespace MM
{

struct Vec3
{
    float x, y, z;
};

struct Quaternion
{
    float w, x, y, z;
};

struct Bno055Data
{
    Vec3 accel{};
    Vec3 gyro{}; // PID will use gyro for yaw control
    Vec3 euler{};
    Quaternion quat{};
};

}  // namespace MM
