#include <vector>

#include "grid.h"
#include "solver.h"

class BackSolve : public Solver
{
private:
  static Grid recur_helper(Grid g, std::vector<Coordinate> coords)
  {
    // g.display_values();
    if (coords.empty())
    {
      return g;
    }

    Coordinate c = coords.back();
    coords.pop_back();

    for (dtype guess = 1; guess <= g.size(); guess++)
    {
      g[c.r][c.c] = guess;
      if (g.is_possible())
      {
        Grid ret = recur_helper(g, coords);
        if (ret.validate())
    {
      return ret;
    }
      }
    }

    //if none of approach found
    g[c.r][c.c] = -1;
    return g;
  }

public:
  Grid seq_solve(Grid g) override
  {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    return recur_helper(g, coords);
  }
  Grid par_solve(Grid g) override
  {

    // implement this later after BFS for checkpoint
    return Grid();
  }
};