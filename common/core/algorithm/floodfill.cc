#include "floodfill.h"

namespace MM
{

void Floodfill::update() {
    // Update logic for the floodfill seq
};

void Floodfill::mark_wall(std::queue<Position> position_queue, int a,
                          bool has_wall)
{
    if (hasWall)
    {
        // Set the bit at position 'dir' to 1 (Wall is present)
        walls[x][y] |= (1 << dir);
    }
    else
    {
        // Clear the bit at position 'dir' to 0 (No wall is present)
        walls[x][y] &= ~(1 << dir);
    }
};

bool Floodfill::has_known_wall(std::deque<Position> path, int a)
{
    // Check the bit at position 'a' in the walls array for cell (x,y)

    return true;
};

bool Floodfill::track_path()
{
    // Logic to track the path from start to goal

    return true;
};

bool Floodfill::flood()
{
    // Logic to perform the floodfill algorithm where it's flood through the maze

    return true;
};

bool Floodfill::check_wall()
{
    // Logic to check if the maze has been fully explored

    return true;
};

bool Floodfill::init_wall()
{
    // Logic to initialize the wall information for the maze

    return true;
};

}  // namespace MM
