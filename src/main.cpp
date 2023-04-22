#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>

#include "backsolve.cpp"
#include "bfs.cpp"
#include "util.h"
#include "timing.h"

int main(int argc, const char **argv)
{
  std::vector<Grid> all_grids;
  Solver *solver;
  StartupOptions options = parseOptions(argc, argv);
  std::string input_file = "testbench/" + options.input_file;
  file_to_grids(input_file, all_grids,options.grid_cnt);
  std::cout << all_grids.size() << " puzzles are being solved" << std::endl;

  solver = options.solver;

  Timer t;
  t.reset();

  Grid solved_grid;
  #pragma parallel for
  for (Grid grid : all_grids)
  {
    if (options.run_parallel)
    {
      solved_grid = solver->par_solve(grid);
    }
    else
    {
      solved_grid = solver->seq_solve(grid);
    }
    int ans = solved_grid.validate();
    if (ans == false)
    {
      std::cout << "This puzzle is solved incorrectly" << std::endl;
    }
  }
  std::cout << "Puzzle was solved " << t.elapsed() << " s" << std::endl;
  return 0;
}
