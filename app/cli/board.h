#pragma once

#include "../../common/core/cli/cli.h"

namespace MM
{

struct Board
{
    Cli& cli;
};

bool board_init(void);
Board& get_board(void);

}  // namespace MM