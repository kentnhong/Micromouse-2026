#include "mms.h"
#include <iostream>

namespace MM
{

Simulation::Simulation()
{
}

void Simulation::logMsg(const std::string& text)
{
    std::cerr << text << std::endl;
}

void Simulation::setText(int x, int y, const std::string& text)
{
    std::cout << "setText " << x << " " << y << " " << text << std::endl;
}

void Simulation::setColor(int x, int y, char color)
{
    std::cout << "setColor " << x << " " << y << " " << color << std::endl;
}

bool Simulation::wallFront()
{
    std::cout << "wallFront" << std::endl;
    std::string response;
    std::cin >> response;
    return response == "true";
}

bool Simulation::wallRight()
{
    std::cout << "wallRight" << std::endl;
    std::string response;
    std::cin >> response;
    return response == "true";
}

bool Simulation::wallLeft()
{
    std::cout << "wallLeft" << std::endl;
    std::string response;
    std::cin >> response;
    return response == "true";
}

void Simulation::moveForward()
{
    std::cout << "moveForward" << std::endl;
    std::string response;
    std::cin >> response;
}

void Simulation::turnRight()
{
    std::cout << "turnRight" << std::endl;
    std::string response;
    std::cin >> response;
}

void Simulation::turnLeft()
{
    std::cout << "turnLeft" << std::endl;
    std::string response;
    std::cin >> response;
}

void Simulation::turnAround()
{
    sim.turnRight();
    sim.turnRight();
}

}  // namespace MM
