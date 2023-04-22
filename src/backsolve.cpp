#include <vector>

#include "grid.h"
#include "solver.h"
#include <tuple>
class BackSolve : public Solver
{
private:
   std::tuple<Grid,bool>  recur_helper(Grid g)
  {
    // g.display_values();
    int row, col;
    if (!g.find_next_empty_cell(row, col))
    {
      return std::make_tuple(g,true);
    }

    // std::cout << row << col << std::endl;
    // exit(1);

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

    //if none of approach found
    g[row][col] = -1;
    return std::make_tuple(g,false);
  }


/*
  std::tuple<Grid,bool>  recur_helper_par(Grid g, std::vector<Coordinate> coords)
  {
    // g.display_values();
    if (coords.empty())
    {
      return std::make_tuple(g,true);
    }

    Coordinate c = coords.back();
    coords.pop_back();

    std::tuple<Grid, bool> ret;
    // #pragma omp for
    for (dtype guess = 1; guess <= g.size(); guess++)
    {
      g[c.r][c.c] = guess;
      if (g.is_possible())
      {
        #pragma omp task shared(ret)
        
        ret = recur_helper(g, coords);
        // Check if puzzle is solved completely
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
*/
public:
  Grid seq_solve(Grid g) override
  {
    // std::vector<Coordinate> coords = g.find_all_empty_cells();
    return std::get<0>(recur_helper(g));
  }
  
  Grid par_solve(Grid g) override
  {
    /*
std::vector<Coordinate> coords = g.find_all_empty_cells();
    return std::get<0>(recur_helper_par(g, coords));
    */
    return Grid();
  }
};