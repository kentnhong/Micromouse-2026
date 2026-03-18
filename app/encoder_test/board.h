/**
* @file board.h
* @brief BSP interface for Encoder testing
* @author Bex Saw
* @date 03/18/2026
*/

#pragma once
#include "encoder.h"

namespace MM
{

struct Board
{
    Encoder& encoder;
};

bool bsp_init(void);
Board& get_board(void);
}  // namespace MM