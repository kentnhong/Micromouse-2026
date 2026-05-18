/**
* @file floodfill.h
* @author Bex Saw
* @brief Floodfill algorithm implementaiton for maze solving
* @version 0.1
*/

#pragma once
#include <algorithm>
#include <cstdint>
#include "ircontroller.h"

namespace MM
{
class Floodfill
{
public:
    enum class Mode
    {
        SEARCH,
        ZOOMING
    };

    Floodfill();

    /**
    * @brief Floodfill algorithm to update the distance values of the maze squares
    */
    void update();

    /**
    * @brief Sets the mode of the floodfill algorithm
    * @param next_mode The mode to set (SEARCH or ZOOMING)
    */
    void set_mode(Mode next_mode);

    /**
    * @brief Get the current mode and state of the floodfill algorithm
    * @return The current mode of the algorithm
    */
    Mode get_mode() const
    {
        return mode;
    }

    /**
    * @brief Set the sensor data for the current position of the mouse in the maze
    * @param front_wall, right_wall, left_wall detecting true or false 
    */
    void set_sensor_data(bool front_wall, bool right_wall, bool left_wall);

    /**
    * @brief Get the next move for the mouse based on the current state of the floodfill algorithm
    * @return A character representing the next move 
    * ('F' for forward, 'R' for turn right, 'L' for turn left, 'U' for U-turn)
    */
    char get_next_move();

    /**
    * @brief Get the calculated distance value for a specific cell
    * @return The distance to the goal
    */
    unsigned char get_distance(int x, int y) const
    {
        return maze[x][y];
    }

    /**
    * @brief Process the IR sensor data to update the floodfill algo
    * @param ir_vals The IR sensor values to process (raw values from the sensors)
    */
    void process_ir_data(const IrValues& ir_vals);

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
    * @brief Marks a wall in the maze based on the sensor data
    * @param cell The current position of the mouse in the maze
    * @param dir The direction in which to mark the wall
    * @param has_wall Boolean indicating whether there is a wall in that direction
    * @return The direction in which the wall was marked
    */
    Direction mark_wall(Position cell, Direction dir, bool has_wall);

    /**
    * @brief Checks if a wall is known at a specific position and direction
    * @param cell The position to check
    * @param dir The direction to check
    * @return Boolean indicating if a wall is known
    */
    bool known_wall(Position cell, Direction dir);

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
    * @brief Checks if a wall exists at a specific position and direction
    * @param cell The position to check
    * @param dir The direction to check
    * @return Boolean indicating if a wall exists
    */
    bool check_wall(Position cell, Direction dir);

    /**
    * @brief Initializes the wall information for the maze
    * @return Boolean indicating if initialization was successful
    */
    bool init_wall();

    /**
    * @brief Updates the search mode of the algorithm
    */
    void update_search();

    /**
    * @brief Updates the zooming mode of the algorithm
    */
    /**
    * @brief Converts a relative turn to an absolute direction based on the current direction
    * @param relative_turn The relative turn (0 for forward, 1 for right, 2 for backward, 3 for left)
    * @return The absolute direction corresponding to the relative turn
    */
    Direction relative_direction(int relative_turn) const;

    static constexpr int MAZE_SIZE = 16;
    unsigned char maze[MAZE_SIZE][MAZE_SIZE];
    char walls[MAZE_SIZE][MAZE_SIZE];

    int dir{0};
    Mode mode{Mode::SEARCH};
    int current_x{0};
    int current_y{0};

    // Default values for the sensor data
    bool sensor_front_wall{false};
    bool sensor_right_wall{false};
    bool sensor_left_wall{false};
};

}  // namespace MM
