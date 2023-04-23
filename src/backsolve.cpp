#include <vector>

#include "grid.h"
#include "solver.h"
#include <tuple>
class BackSolve : public Solver
{
private:
  std::tuple<Grid, bool> recur_helper(Grid g)
  {
    // g.display_values();
    int row, col;
    if (!g.find_next_empty_cell(row, col))
    {
      return std::make_tuple(g, true);
    }

    for (dtype guess = 1; guess <= g.size(); guess++)
    {
      g[row][col] = guess;
      if (g.is_possible())
      {
        auto ret = recur_helper(g);
        if (std::get<1>(ret))
        {
          return ret;
        }
      }
    }

    // if none of approach found
    g[row][col] = -1;
    return std::make_tuple(g, false);
  }

  std::tuple<Grid, bool> recur_helper_par(Grid g)
  {
    // g.display_values();
    int row, col;
    Grid solution_grid;
    bool found_solution=false;
    if (!g.find_next_empty_cell(row, col))
    {
      return std::make_tuple(g, true);
    }

    for (dtype guess = 1; guess <= g.size(); guess++)
    {
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
    if (found_solution){
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
    return std::get<0>(recur_helper(g));
  }

  Grid par_solve(Grid g) override
  {
    
    return std::get<0>(recur_helper_par(g));
    
  }
};