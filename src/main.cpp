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
  file_to_grids(input_file, all_grids);
  // file_to_grids("testbench/simple-500", all_grids);
  // file_to_grids("testbench/4x4", all_grids);

  // Grid a(4, input);
  // a.display_values();
  solver = options.solver;

  Timer t;
  t.reset();
  // for (Grid grid: all_grids){
  //   solved_grid = b.par_solve(grid);
  //   // solved_grid = b.seq_solve(grid);
  //   int ans = f.validate();
  //   if (ans == false){
  //     std::cout << "This puzzle is solved incorrectly" << std::endl;

  //   }
  // }
  Grid a = all_grids[0];
  Grid solved_grid;
  if (options.run_parallel)
  {
    solved_grid = solver->par_solve(a);
  }
  else
  {
    solved_grid = solver->seq_solve(a);
  }
  int ans = solved_grid.validate();
  solved_grid.display_values();
  if (ans == false)
  {
    std::cout << "This puzzle is solved incorrectly" << std::endl;
  }
  std::cout << "Puzzle was solved " << t.elapsed() << " s" << std::endl;

  return 0;
}
