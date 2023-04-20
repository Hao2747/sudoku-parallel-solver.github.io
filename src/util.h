#include <fstream>
#include <string.h>
#include "solver.h"


struct StartupOptions
{
    std::string input_file = "simple-500";
    Solver *solver = new BFS();
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
                if (strcmp(argv[i + 1], "BFS") == 0){
                    rs.solver = new BFS();
                }
                else if (strcmp(argv[i + 1], "DFS") == 0){
                    rs.solver = new BackSolve();
                }
            }
        }
    }
    return rs;
}

void file_to_grids(std::string filename, std::vector<Grid> &grids)
{
    std::ifstream inputFile(filename); // open file for input
    std::string line;

    dtype grid_size = 0;

    if (inputFile.is_open())
    { // check if file is open
        while (std::getline(inputFile, line, '\n'))
        { // read lines until end of file
            if (grid_size == 0)
            {
                grid_size = std::sqrt(line.length()); // a line has length -1 number to fill in suduku
                // printf("line as %s, size as %d\n", line.c_str(), grid_size);
                std::cout << "line as " << line << std::endl;
                std::cout << "size as " << grid_size << std::endl;
            }
            std::vector<std::vector<int>> matrix(grid_size, std::vector<int>(grid_size, -1));

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
        }
        inputFile.close(); // close the file
    }
    else
    {
        std::cerr << "Unable to open file" << std::endl;
    }
}
