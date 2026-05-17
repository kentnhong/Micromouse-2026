#include "floodfill.h"
#include "sim/mms.h"

using namespace MM;
int main()
{
    Simulation sim;
    sim.logMsg("Running Floodfill in MMS Simulator...");

    MM::Floodfill mouseAlgo;

    while (1)
    {
        // 1. Read sensors from the MMS Simulator
        bool f_wall = sim.wallFront();
        bool r_wall = sim.wallRight();
        bool l_wall = sim.wallLeft();

        // 2. Pass reality to your algorithm
        mouseAlgo.set_sensor_data(f_wall, r_wall, l_wall);

        // 3. Let it crunch the numbers and flood the maze
        mouseAlgo.update();

        // Color and print the maze distances to the screen!
        for (int x = 0; x < 16; ++x)
        {
            for (int y = 0; y < 16; ++y)
            {
                unsigned char dist = mouseAlgo.get_distance(x, y);
                // Print the distance text on the cell
                sim.setText(x, y, std::to_string(dist));

                // Color the center goal green
                if ((x == 7 || x == 8) && (y == 7 || y == 8))
                {
                    sim.setColor(x, y, 'G');
                }
            }
        }

        // 4. Ask the algorithm which way to go
        char move = mouseAlgo.get_next_move();

        std::string moveStr(1, move);
        sim.logMsg("Algorithm chose to move: " + moveStr);

        // 5. Execute the move mechanically in the simulator
        if (move == 'F')
        {
            sim.moveForward();
        }
        else if (move == 'R')
        {
            sim.turnRight();
            sim.moveForward();
        }
        else if (move == 'L')
        {
            sim.turnLeft();
            sim.moveForward();
        }
        else if (move == 'U')
        {
            sim.turnAround();
            sim.moveForward();
        }
    }

    return 0;
}
