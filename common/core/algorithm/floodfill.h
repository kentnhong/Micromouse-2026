/**
* @file floodfill.h
* @author Bex Saw
* @brief Floodfill algorithm implementaiton for maze solving
* @version 0.1
*/

#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <iterator>
#include <queue>

/**
* @note INPUT TO THE ALGORITHM - navigation data from the mouse's sensors indicating the presence of walls in the four cardinal directions 
        (NORTH, EAST, SOUTH, WEST) at each position in the maze.
* @note OUTPUT OF THE ALGORITHM - a path from the start position to the goal position, 
*       which can be used by the mouse to navigate through the maze.
*/

namespace MM
{
class Floodfill
{
public:
    /**
    * @brief Floodfill algorithm to update the distance values of the maze squares
    */
    void update();

private:
    /**
    * @brief Enum for the four cardinal directions
    * @note NORTH = 0, EAST = 1, SOUTH = 2, WEST = 3
    */
    enum class Direction : uint8_t
    {
        NORTH = 0,
        EAST,
        SOUTH,
        WEST
    };

    /**
    * @brief Position struct to represent coordinates in the maze
    */
    struct Position
    {
        int x{0};
        int y{0};
    };

    /**
    * @brief Marks a wall in the maze
    * @param a Direction of the wall
    * @param has_wall Boolean indicating if a wall exists
    */
    Direction mark_wall(std::queue<Position> pos, int a, bool has_wall);

    /**
    * @brief Checks if a wall is known at a specific position and direction
    * @param a Direction to check
    * @return Boolean indicating if a wall is known
    */
    bool known_wall(std::queue<Position> path, int a);

    /**
    * @brief Tracks the path from the start to the goal
    * @return Boolean indicating if a path was found
    */
    bool track_path();

    /**
    * @brief Performs the floodfill algorithm
    * @return Boolean indicating if the algorithm completed successfully
    */
    bool flood();

    /**
    * @brief Checks if the maze has been fully explored
    * @return Boolean indicating if the maze is fully explored
    */
    bool check_wall();

    /**
    * @brief Initializes the wall information for the maze
    * @return Boolean indicating if initialization was successful
    */
    bool init_wall();

    /// *********************************************************** ///

    // Queue for floodfill algorithm
    std::queue<Position> pos;

    // Deque for path tracking
    std::queue<Position> path;

    /// SIZE: 16x16 maze
    static constexpr int MAZE_SIZE = 16;

    int dir{0};
    int turn{0};
    int target{0};
    int num{14};
    int min{0};
    int max{0};
    int count{0};
    std::array<int, 4> distance;

    unsigned char maze[MAZE_SIZE][MAZE_SIZE];
    unsigned char check[MAZE_SIZE][MAZE_SIZE];
    char walls[MAZE_SIZE][MAZE_SIZE];

    // position of the mouse in the maze using in track_path();
    // where xuperX is the x-coordinate and xuperY is the y-coordinate
    int xuperX{0};
    int xuperY{0};
};
}  // namespace MM