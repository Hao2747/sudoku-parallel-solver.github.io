#include <fstream>
#include "main.cpp"


void file_to_grids(std::string filename, std::vector<Grid> &grids){
    std::ifstream inputFile(filename); // open file for input
    std::string line;
    
    dtype grid_size = 9;

    if (inputFile.is_open()) { // check if file is open
        while (std::getline(inputFile, line)) { // read lines until end of file
            std::vector<std::vector<int>> matrix(9, std::vector<int>(9, -1));

            int idx = 0;
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    if (line[idx] != '.') {
                        matrix[i][j] = line[idx] - '0'; // convert character to integer
                    }
                    idx++;
                }
            }
            
            grids.emplace_back(Grid(grid_size, matrix));
        }
        inputFile.close(); // close the file
    }
    else {
        std::cerr << "Unable to open file" << std::endl;
    }
}