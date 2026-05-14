/**
* @file mms.h
* @author Bex Saw
* @brief Interface for the MMS Simulator to interact with the floodfill algorithm
* @version 0.1
*/

#pragma once

#include <string>

namespace MM
{
class Simulation
{
public:
    explicit Simulation();

    /**
    * @brief This is all of the mock function for testing
    * These will eventually be replaced with real sensor data 
    * and motor control functions that interact with the hardware of the mouse
    */
    void logMsg(const std::string& text);
    bool wallFront();
    bool wallRight();
    bool wallLeft();
    void moveForward();
    void turnRight();
    void turnLeft();
    void turnAround();
    void setText(int x, int y, const std::string& text);
    void setColor(int x, int y, char color);
};
}  // namespace MM
