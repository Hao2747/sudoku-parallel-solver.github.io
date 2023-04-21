#include <vector>

#include "grid.h"
#include "solver.h"
#include <tuple>
class BackSolve : public Solver
{
private:
   std::tuple<Grid,bool>  recur_helper(Grid g, std::vector<Coordinate> coords)
  {
    // g.display_values();
    if (coords.empty())
    {
      return std::make_tuple(g,true);
    }

    Coordinate c = coords.back();
    coords.pop_back();

    for (dtype guess = 1; guess <= g.size(); guess++)
    {
      g[c.r][c.c] = guess;
      if (g.is_possible())
      {
        auto ret = recur_helper(g, coords);
        if (std::get<1>(ret))
    {
      return ret;
    }
      }
    }

    //if none of approach found
    g[c.r][c.c] = -1;
    return std::make_tuple(g,false);
  }

public:
  Grid seq_solve(Grid g) override
  {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    return std::get<0>(recur_helper(g, coords));
  }
  Grid par_solve(Grid g) override
  {

    // implement this later after BFS for checkpoint
    return Grid();
  }
};