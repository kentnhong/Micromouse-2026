#pragma once
#include "floodfill.h"
#include "ircontroller.h"
// #include "bno055_imu.h"

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