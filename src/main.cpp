#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>


#include "backsolve.cpp"
#include "bfs.cpp"
#include "util.h"
#include "timing.h"



typedef int dtype; 





class Solver {
public:
  virtual Grid seq_solve(Grid g) = 0;
  virtual Grid par_solve(Grid g) = 0;
  
};




int main()
{
    std::vector<Grid> all_grids;
    // file_to_grids("testbench/medium-100", all_grids);
    file_to_grids("testbench/simple-500", all_grids);

    Timer t;
    t.reset();
    BFS b;
    for (Grid grid: all_grids){
      Grid f = b.par_solve(grid); 
      // Grid f = b.seq_solve(grid); 
      int ans = f.validate();
      if (ans == false){
        std::cout << "This puzzle is solved incorrectly" << std::endl;

      }
    }
    std::cout << "Puzzle was solved " << t.elapsed() << " s" << std::endl;
    

    
    
    
    
    return 0;
}
