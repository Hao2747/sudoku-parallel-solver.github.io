#include <fstream>
#include <string.h>
#include "solver.h"
#include "cp_backsolve.h"
#include "bfs.h"
#include "backsolve.h"
#include "cp_no_barrier.h"

struct StartupOptions
{
    std::string input_file = "4x4";
    Solver *solver = nullptr;
    // bool run_parallel = true;
    // At most n grids is being read from the input file. -1 means read all grids from file
    int grid_cnt = -1;
    std::vector<std::string> modes;
};

StartupOptions parseOptions(int argc, const char **argv)
{
    StartupOptions rs;
    for (int i = 1; i < argc; i++)
    {
        // For those with arguments
        if (i < argc - 1)
        {
            if (strcmp(argv[i], "-in") == 0)
            {
                rs.input_file = argv[i + 1];
            }

            if (strcmp(argv[i], "-solver") == 0)
            {
                if (strcmp(argv[i + 1], "BFS") == 0)
                {
                    rs.solver = new BFS();
                }
                else if (strcmp(argv[i + 1], "DFS") == 0)
                {
                    rs.solver = new BackSolve();
                }
                else if (strcmp(argv[i + 1], "CP_Backsolve") == 0)
                {
                    rs.solver = new CP_Backsolve();
                }
                else if (strcmp(argv[i + 1], "CP_No_Barrier") == 0)
                {
                    rs.solver = new CP_No_Barrier();
                }
                
                // Startup options. Is not included in startup option to avoid
                // instantiating a not used object
            }
        }

        if ((strcmp(argv[i], "-par")) == 0)
        {
            rs.modes.emplace_back("par");
            std::cout << "Running in parallel" << std::endl;
        }
        else if ((strcmp(argv[i], "-seq")) == 0)
        {
            rs.modes.emplace_back("seq");
            std::cout << "Running in sequential" << std::endl;
        }
        else if ((strcmp(argv[i], "-both")) == 0)
        {
            rs.modes.emplace_back("seq");
            rs.modes.emplace_back("par");
            std::cout << "Running both seq/par and compare their performance" << std::endl;
        }

        if (strcmp(argv[i], "-i") == 0)
        {
            rs.grid_cnt = atoi(argv[i + 1]);
        }
    }

    // Startup option
    if (rs.modes.empty())
    {
        rs.modes.emplace_back("seq");
        std::cout << "Running in parallel" << std::endl;
    }
    if (!rs.solver)
    {
        rs.solver = new CP_No_Barrier();
    }
    return rs;
}

void file_to_grids(std::string filename, std::vector<Grid> &grids, int grid_cnt = -1)
{
    std::ifstream inputFile(filename); // open file for input
    std::string line;

    int line_cnt = 0;
    dtype grid_size;

    if (inputFile.is_open())
    { // check if file is open
        while (std::getline(inputFile, line, '\n'))
        { // read lines until end of file
            if (line_cnt < grid_cnt || grid_cnt == -1)
            {
                // when reading the line first time, determine the grid size
                if (line_cnt == 0)
                {
                    grid_size = std::sqrt(line.length()); // a line has length -1 number to fill in suduku
                    // printf("line as %s, size as %d\n", line.c_str(), grid_size);
                    // std::cout << "line as " << line << std::endl;
                    // std::cout << "size as " << grid_size << std::endl;
                }
                std::vector<std::vector<int>> matrix(grid_size, std::vector<int>(grid_size, UNASSIGNED));

                int idx = 0;
                for (int i = 0; i < grid_size; i++)
                {
                    for (int j = 0; j < grid_size; j++)
                    {
                        if (line[idx] != '.')
                        {
                            matrix[i][j] = line[idx] - '0'; // convert character to integer
                        }
                        idx++;
                    }
                }

                grids.emplace_back(Grid(grid_size, matrix));
                line_cnt++;
            }
            else
            {
                break;
            }
        }
        inputFile.close(); // close the file
    }
    else
    {
        std::cerr << "Unable to open file" << std::endl;
    }
}
