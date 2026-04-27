#include "floodfill.h"

namespace MM
{

void Floodfill::update()
{
    // Update logic for the floodfill sequence

    // Initialize the walls of the maze
    Floodfill::init_wall();

    /// TODO: Implement the floodfill algorithm to update the distance values of the maze squares
}

/// *************************************************************************///

Floodfill::Direction Floodfill::mark_wall(std::queue<Position> pos, int a,
                                          bool has_wall)
{
    /// NOTE: has_wall indicates whether there is a wall in the direction 'a' from the current position
    ///       If has_wall is true, we mark the wall in the walls array. If has_wall is false, we do not mark it,
    ///       indicating that there is no wall in that direction.

    // Based on the direction 'a' mark the wall
    switch (static_cast<Direction>(a))
    {
        case Direction::NORTH:
            // Handle NORTH direction
            if (has_wall)
            {
                // Mark the wall in the NORTH direction
                walls[pos.front().x][pos.front().y] |= (1 << 0);
            }
            break;
        case Direction::EAST:
            // Handle EAST direction
            if (has_wall)
            {
                // Mark the wall in the EAST direction
                walls[pos.back().x][pos.back().y] |= (1 << 1);
            }
            break;
        case Direction::SOUTH:
            // Handle SOUTH direction
            if (has_wall)
            {
                // Mark the wall in the SOUTH direction
                walls[pos.front().x][pos.front().y] |= (1 << 2);
            }
            break;
        case Direction::WEST:
            // Handle WEST direction
            if (has_wall)
            {
                // Mark the wall in the WEST direction
                walls[pos.front().x][pos.front().y] |= (1 << 3);
            }
            break;
    }
    return Direction::NORTH;  // Default case
}

bool Floodfill::known_wall(std::queue<Position> path, int a)
{
    // Check the bit at position 'a' in the walls array for cell (x,y)
    return (walls[path.front().x][path.front().y] & (1 << a)) != 0;
}

bool Floodfill::track_path()
{
    // (xuperX, xuperY) represents the current position of the mouse in the maze
    // 0 = NORTH, 1 = EAST, 2 = SOUTH, 3 = WEST

    // Logic to track the path from start to goal
    if (dir == 0)
    {
        xuperY++;  // Y because we are moving NORTH
    }
    else if (dir == 1)
    {
        xuperX++;  // X because we are moving EAST
    }
    else if (dir == 2)
    {
        xuperY--;  // Y because we are moving SOUTH
    }
    else if (dir == 3)
    {
        xuperX--;  // X because we are moving WEST
    }
    else if (xuperX == 7 && xuperY == 7)
    {
        return true;  // Goal reached
    }

    return true;
}

bool Floodfill::flood()
{
    /// NOTE: At the diagram level it will look like this:
    ///      1. Start at the initial position (0,0) and enqueue it
    ///      2. While the queue is not empty:
    ///         a. Dequeue the front position from the queue
    ///         b. For each of the four directions (NORTH, EAST, SOUTH, WEST):
    ///            i. Check if there is a known wall in that direction using known_wall function
    ///            ii. If there is no wall, enqueue the adjacent position in that direction
    ///      3. Repeat until the queue is empty or the goal is reached

    pos.push({xuperX, xuperY});

    while (!pos.empty())
    {
        // Get the current position from the queue
        Position current = pos.front();

        // Remove the current position from the queue
        pos.pop();

        // Check all four directions from the current position
        if (known_wall(pos, static_cast<int>(Direction::NORTH)))
        {
            // If there is no wall to the NORTH, enqueue the position to the NORTH
            pos.push({current.x, current.y + 1});
        }
        if (known_wall(pos, static_cast<int>(Direction::EAST)))
        {
            // If there is no wall to the EAST, enqueue the position to the EAST
            pos.push({current.x + 1, current.y});
        }
        if (known_wall(pos, static_cast<int>(Direction::SOUTH)))
        {
            // If there is no wall to the SOUTH, enqueue the position to the SOUTH
            pos.push({current.x, current.y - 1});
        }
        if (known_wall(pos, static_cast<int>(Direction::WEST)))
        {
            // If there is no wall to the WEST, enqueue the position to the WEST
            pos.push({current.x - 1, current.y});
        }
    }
    return true;
}

bool Floodfill::check_wall()
{
    // TODO: Implement a logic where it's of the data from the sensors to check if the maze has been fully explored.
    //       Fetch directly from navigation system
    return true;
}

bool Floodfill::init_wall()
{
    /// NOTE: Logic to initialize the wall information for the maze

    // Clear all walls and distances
    for (auto& row : walls)
    {
        std::fill(std::begin(row), std::end(row),
                  0);  // Initialize all walls to 0 (no walls)
    }

    // Set North and South outer boundaries
    for (int x{0}; x < MAZE_SIZE; x++)
    {
        walls[x][0] |= (1 << 2);              // SOUTH 2
        walls[x][MAZE_SIZE - 1] |= (1 << 0);  // NORTH 0
    }

    // Set East and West outer boundaries
    for (int y{0}; y < MAZE_SIZE; y++)
    {
        walls[0][y] |= (1 << 3);              // WEST 3
        walls[MAZE_SIZE - 1][y] |= (1 << 1);  // EAST 1
    }

    return true;
}

}  // namespace MM
