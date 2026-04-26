/**
* @file floodfill.h
* @author Bex Saw
* @brief Floodfill algorithm implementaiton for maze solving
* @version 0.1
*/

#pragma once
#include <cstdio>
#include <cstdlib>
#include <deque>

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
    * @brief Marks a wall in the maze
    * @param a Direction of the wall
    * @param has_wall Boolean indicating if a wall exists
    */
    void mark_wall(std::deque<Position> pos, int a, bool has_wall);

    /**
    * @brief Checks if a wall is known at a specific position and direction
    * @param a Direction to check
    * @return Boolean indicating if a wall is known
    */
    bool has_known_wall(std::queue<Position> path, int a);

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

    /**
    * @brief Position struct to represent coordinates in the maze
    */
    struct Position
    {
        int x{0};
        int y{0};
    };

    // Queue for floodfill algorithm
    std::deque<Position> pos;

    // Deque for path tracking
    std::deque<Position> path;

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

    // TODO: Populate a 2D array to represent the maze and walls
    unsigned char maze[MAZE_SIZE][MAZE_SIZE];
};
}  // namespace MM