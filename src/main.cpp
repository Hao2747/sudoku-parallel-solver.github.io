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
  std::cout << options.grid_cnt << std::endl;
  handle_files(input_file, all_grids, options.grid_cnt);
  std::cout << all_grids.size() << " puzzles are being solved" << std::endl;

  solver = options.solver;

  Timer t;

  Grid solved_grid;
  std::vector<double> time_to_solve;
  // if (options.mode == "both"){}
  for (auto mode : options.modes)
  {
    t.reset();
    for (Grid grid : all_grids)
    {
      if (mode == "par")
      {
        solved_grid = solver->par_solve(grid);
      }
      else if (mode == "seq")
      {
        solved_grid = solver->seq_solve(grid);
      }
      int ans = solved_grid.validate();
      if (ans == false)
      {
        solved_grid.display_values();
        std::cout << "This puzzle is solved incorrectly" << std::endl;
      }
    }
    double complete_time = t.elapsed();
    time_to_solve.emplace_back(complete_time);
  }
  if (time_to_solve.size() == 1){
    std::cout << options.modes[0] << ": Puzzle was solved " << time_to_solve[0]<< " s" << std::endl;
  }
  else if (time_to_solve.size() == 2){
    std::cout << "Sequential: Puzzle was solved " << time_to_solve[0]<< " s" << std::endl;
    std::cout << "Parallel: Puzzle was solved " << time_to_solve[1]<< " s" << std::endl;
    std::cout << "Speedup: " << time_to_solve[0] / time_to_solve[1] << "x" << std::endl;
  }

  return 0;
}
