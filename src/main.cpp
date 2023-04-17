#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>



#include "bfs.cpp"
#include "util.h"

#ifndef GRID_H
#define GRID_H
#endif 


typedef int dtype; 





class Solver {
public:
  virtual Grid seq_solve(Grid g) = 0;
  virtual Grid par_solve(Grid g) = 0;
  
};




int main()
{
    std::vector<Grid> all_grids;
    file_to_grids("testbench/simple-50.txt", all_grids);
    Grid a = all_grids[0];
    a.display_values();

    BFS b;
    int ans;
    Grid f = b.seq_solve(a); 

    f.display_values();
    
    ans = f.validate();
    std::cout << ans << std::endl;
    
    
    
    return 0;
}
