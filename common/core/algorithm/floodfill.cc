#include "floodfill.h"
#include <algorithm>
#include <iterator>
#include <queue>

namespace MM
{

Floodfill::Floodfill()
{
    Floodfill::init_wall();
}

void Floodfill::update()
{
    switch (mode)
    {
        case Mode::SEARCH:
            update_search();
            break;
        case Mode::ZOOMING:
            break;
    }
}

char Floodfill::get_next_move()
{
    constexpr int kDirectionCount{4};
    constexpr int dx[kDirectionCount]{0, 1, 0, -1};
    constexpr int dy[kDirectionCount]{1, 0, -1, 0};

    Position current{current_x, current_y};

    int best_dir{dir};
    unsigned char best_distance{255};

    // Explore each of the four possible directions to find the best move based on the distance values in the maze
    for (int next_dir{0}; next_dir < kDirectionCount; ++next_dir)
    {
        // Check if there is a wall in the current direction from the current position
        const auto direction{static_cast<Direction>(next_dir)};

        // If there is a known wall in this direction, skip it
        if (known_wall(current, direction))
        {
            continue;
        }

        // Calculate the coordinates of the neighboring cell in the current direction
        const int next_x{current.x + dx[next_dir]};
        const int next_y{current.y + dy[next_dir]};

        // Check if the neighboring cell is within the bounds of the maze
        if (next_x < 0 || next_x >= MAZE_SIZE || next_y < 0 ||
            next_y >= MAZE_SIZE)
        {
            continue;
        }

        // Update the best direction if the neighboring cell has a smaller distance value than the current best distance
        if (maze[next_x][next_y] < best_distance)
        {
            best_distance = maze[next_x][next_y];
            best_dir = next_dir;
        }
    }

    // Calculate the relative turn needed to face the best direction and return the corresponding move
    const int turn{(best_dir - dir + kDirectionCount) % kDirectionCount};

    // Update the current direction to the best direction
    dir = best_dir;

    // Track the path for readings the next move
    track_path();

    // Return the move corresponding to the relative turn
    if (turn == 0)
    {
        return 'F';
    }
    if (turn == 1)
    {
        return 'R';
    }
    if (turn == 2)
    {
        return 'U';
    }
    return 'L';
}

void Floodfill::update_search()
{
    Position current{current_x, current_y};
    mark_wall(current, relative_direction(0), sensor_front_wall);
    mark_wall(current, relative_direction(1), sensor_right_wall);
    mark_wall(current, relative_direction(3), sensor_left_wall);
    flood();
}

Floodfill::Direction Floodfill::relative_direction(int relative_turn) const
{
    return static_cast<Direction>((dir + relative_turn) % 4);
}

/// @private functions

Floodfill::Direction Floodfill::mark_wall(Position cell, Direction dir,
                                          bool has_wall)
{
    /// NOTE: has_wall indicates whether there is a wall in the direction 'a' from the current position
    ///       If has_wall is true, we mark the wall in the walls array. If has_wall is false, we do not mark it,
    ///       indicating that there is no wall in that direction.

    if (has_wall)
    {
        walls[cell.x][cell.y] |= (1 << static_cast<int>(dir));
    }

    if (!has_wall)
    {
        return dir;
    }

    constexpr int kDirectionCount{4};
    constexpr int dx[kDirectionCount]{0, 1, 0, -1};
    constexpr int dy[kDirectionCount]{1, 0, -1, 0};

    const int wall_dir{static_cast<int>(dir)};
    const int neighbor_x{cell.x + dx[wall_dir]};
    const int neighbor_y{cell.y + dy[wall_dir]};

    if (neighbor_x >= 0 && neighbor_x < MAZE_SIZE && neighbor_y >= 0 &&
        neighbor_y < MAZE_SIZE)
    {
        const int opposite_dir{(wall_dir + 2) % kDirectionCount};
        walls[neighbor_x][neighbor_y] |= (1 << opposite_dir);
    }

    return dir;
}

bool Floodfill::track_path()
{
    // (current_x, current_y) represents the current position of the mouse in the maze
    // 0 = NORTH, 1 = EAST, 2 = SOUTH, 3 = WEST

    // Logic to track the path from start to goal
    if (dir == 0)
    {
        current_y++;  // Y because we are moving NORTH
    }
    else if (dir == 1)
    {
        current_x++;  // X because we are moving EAST
    }
    else if (dir == 2)
    {
        current_y--;  // Y because we are moving SOUTH
    }
    else if (dir == 3)
    {
        current_x--;  // X because we are moving WEST
    }
    else if (current_x == 7 && current_y == 7)
    {
        return true;  // Goal reached
    }

    return true;
}

bool Floodfill::flood()
{
    constexpr unsigned char kUnknownDistance{255};
    constexpr int kDirectionCount{
        4};  // Number of possible directions (NORTH, EAST, SOUTH, WEST)
    const int dx[kDirectionCount]{0, 1, 0,
                                  -1};  // Change in x for each direction
    const int dy[kDirectionCount]{1, 0, -1,
                                  0};  // Change in y for each direction

    // Initialize the maze distances to UNKNOWN_DISTANCE
    std::fill_n(&maze[0][0], MAZE_SIZE * MAZE_SIZE, kUnknownDistance);

    // Initialize the queue for the floodfill algorithm
    std::queue<Position> q;

    // Define the goal positions (center of the maze)
    constexpr Position goals[4]{{7, 7}, {7, 8}, {8, 7}, {8, 8}};

    // Set the distance of the goal positions to 0 and enqueue them
    for (const auto& goal : goals)
    {
        // Set the distance of the goal position to 0
        maze[goal.x][goal.y] = 0;
        // Enqueue the goal position
        q.push(goal);
    }

    // Perform the floodfill algorithm using a breadth-first search approach
    while (!q.empty())
    {
        // Dequeue the current position
        const Position current = q.front();
        q.pop();

        // Calculate the distance for the neighboring cells
        const unsigned char next_distance{
            static_cast<unsigned char>(maze[current.x][current.y] + 1)};

        // Explore each of the four possible directions
        for (int dir{0}; dir < kDirectionCount; ++dir)
        {
            // Check if there is a wall in the current direction from the current position
            if (known_wall(current, static_cast<Direction>(dir)))
            {
                continue;
            }

            // Calculate the coordinates of the neighboring cell in current direction
            const int next_x{current.x + dx[dir]};
            const int next_y{current.y + dy[dir]};

            // Check if the neighboring cell is within the bounds of the maze and has
            // not been visited (distance is kUnknownDistance)

            if (next_x < 0 || next_x >= MAZE_SIZE || next_y < 0 ||
                next_y >= MAZE_SIZE || maze[next_x][next_y] != kUnknownDistance)
            {
                continue;
            }

            // Update the distance for the neighboring cell and enqueue it
            maze[next_x][next_y] = next_distance;
            q.push({next_x, next_y});
        }
    }

    return true;
}

bool Floodfill::check_wall(Position cell, Direction dir)
{
    // Does this cell have a wall in this direction?
    return (walls[cell.x][cell.y] & (1 << static_cast<int>(dir))) != 0;
}

bool Floodfill::known_wall(Position cell, Direction dir)
{
    // Check if there is a wall in the direction 'a' from the current position
    return (walls[cell.x][cell.y] & (1 << static_cast<int>(dir))) != 0;
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

void Floodfill::set_mode(Mode next_mode)
{
    mode = next_mode;
}

void Floodfill::set_sensor_data(bool front_wall, bool right_wall,
                                bool left_wall)
{
    sensor_front_wall = front_wall;
    sensor_right_wall = right_wall;
    sensor_left_wall = left_wall;
}

void Floodfill::process_ir_data(const IrValues& ir_vals)
{
    constexpr uint16_t kThresholdFront{
        200};  // TODO: Change based on the readings
    constexpr uint16_t kThresholdSide{
        150};  // TODO: Change based on the readings

    // Process front sensor data (Since we have left and right front sensors)
    bool wall_front = (ir_vals.front_left > kThresholdFront) ||
                      (ir_vals.front_right > kThresholdFront);
    bool wall_right = ir_vals.right > kThresholdSide;
    bool wall_left = ir_vals.left > kThresholdSide;

    // Update the sensor data for the floodfill algorithm
    set_sensor_data(wall_front, wall_right, wall_left);
}
}  // namespace MM
