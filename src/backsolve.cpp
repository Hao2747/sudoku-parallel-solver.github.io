#include <vector>

#include "grid.h"
#include "solver.h"
#include <tuple>
class BackSolve : public Solver
{
private:
  bool recur_helper(Grid &g)
  {
    // g.display_values();
    int row, col;

    // If cannot find more empty cell, puzzle is solved
    // If can find, set row and col of first empty cell
    if (!g.find_next_empty_cell(row, col))
    {
      return true;
    }

    Grid private_g = g;
    for (dtype guess = 1; guess <= g.size(); guess++)
    {
      private_g[row][col] = guess;
      if (private_g.is_possible())
      {
        if (recur_helper(private_g))
        {
          g = private_g;
          return true;
        }
      }
    }

    return false;
  }
  std::tuple<Grid, bool> recur_helper_par(Grid g)
  {

    // g.display_values();
    int row, col;
    Grid solution_grid;
    bool found_solution = false;
    if (!g.find_next_empty_cell(row, col))
    {
      return std::make_tuple(g, true);
    }
    int grid_size = g.size();
    // #pragma omp parallel for
#pragma omp parallel for private(g)
    for (dtype guess = 1; guess <= grid_size; guess++)
    {
      int tid = omp_get_thread_num();
      // if (tid != 0){
      // std::cout << "tid " << tid << ": " << g.display_values_inline() << std::endl;
      // // std::cout << "tid " << tid << std::endl;
      // }
      // g.display_values();
      // printf("tid %d: %s\n", tid, g.display_values_inline().c_str());
      // std::cout << "tid " << tid << ": " << g.display_values_inline() << std::endl;
      g[row][col] = guess;
      if (g.is_possible())
      {
        auto ret = recur_helper_par(g);
        if (std::get<1>(ret))
        {
          solution_grid = std::get<0>(ret);
          found_solution = true;
        }
      }
    }
    if (found_solution)
    {
      return std::make_tuple(solution_grid, true);
    }

    // if none of approach found
    g[row][col] = -1;
    return std::make_tuple(g, false);
  }

public:
  Grid seq_solve(Grid g) override
  {
    // std::vector<Coordinate> coords = g.find_all_empty_cells();
    recur_helper(g);
    return g;
  }

  Grid par_solve(Grid g) override
  {

    return std::get<0>(recur_helper_par(g));
  }
};