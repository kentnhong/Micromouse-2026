/**
* @file mms.h
* @author Bex Saw
* @brief Interface for the MMS Simulator to interact with the floodfill algorithm
* @version 0.1
*/

#pragma once

#include <string>

// Functions to interact with the MMS Simulator
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
