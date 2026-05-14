#pragma once
#include "floodfill.h"

// #include "ir_interface.h"

/**
* @note Inherit from the IR interface class to get access to the latest 
* IR sensor data for decision making.
*
* @note The wholepoint of the Navigation control is for the "floodfill"
* To knows the next move, and for the Navigation class to execute that move
*/

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
    explicit Navigation();

    /**
    * @brief Update the navigation state based on the latest IMU and IR sensor data.
    * @param imu The latest IMU data, used for precise movement control.
    * @param ir The latest IR sensor data, used for maze-solving decisions.
    */
    void update(const Bno055Data& imu, /*const IrData& ir*/);

    State get_current_state() const
    {
        return state;
    }

    ~Navigation();

private:
    State state;
    Floodfill floodfill;

    // Flag for decision phase; true when we are ready to make a new move
    bool at_cell_center{true};
};

}  // namespace MM