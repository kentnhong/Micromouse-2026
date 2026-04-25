<<<<<<< HEAD
#pragma once
#include "floodfill.h"
#include "ircontroller.h"
#include "motioncontroller.h"
// #include "bno055_imu.h" {optional}

namespace MM
{

class Navigation
{
    enum class State : uint8_t
    {
        STRAIGHT = 0,
        TURNING_LEFT,
        TURNING_RIGHT,
        U_TURN,
        STOPPED,
        RESET
    };

public:
    Navigation();

    /**
    * @brief Update the navigation state based on the latest IR sensor data.
    * @param ir The latest IR sensor data, used for maze-solving decisions.
    */
    void update(const IrValues& ir);

    /**
    * @brief Check if the robot has reached the center of a cell.
    * @return true if the robot is at the center of a cell, false otherwise.
    */
    bool has_reached_cell_center() const
    {
        return at_cell_center;
    }

    /**
    * @brief Get the current navigation state.
    * @return The current state of the navigation system.
    */
    State get_current_state() const
    {
        return state;
    }

    /**
    * @brief Notify the navigation system that a move has been completed.
    * @return The current state of the navigation system.
    */
    void complete()
    {
        at_cell_center = true;
        state = State::STOPPED;
    }

    /**
    * @brief Execute the current navigation action using the motion controller.
    * @param motion The motion controller instance.
    * @param ir The latest IR sensor data.
    */
    void execute(MotionController& motion, const IrValues& ir);

    ~Navigation();

private:
    State state;
    Floodfill floodfill;

    // Flag for decision phase; true when we are ready to make a new move
    bool at_cell_center{true};
};
}  // namespace MM
=======
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
>>>>>>> ea30492 (navigation)
