#include <vector>

#include "grid.h"
#include "solver.h"
#include <tuple>

#ifndef BACKSOLVE_H
#define BACKSOLVE_H
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
  bool recur_helper_par(Grid &g)
  {

    int row, col;

    // If cannot find more empty cell, puzzle is solved
    // If can find, set row and col of first empty cell
    if (!g.find_next_empty_cell(row, col))
    {
      return true;
    }
    bool found_solution = false;

#pragma omp parallel shared(found_solution)
    {
      Grid private_g = g;
      #pragma omp cancellation point parallel
      for (dtype guess = 1; guess <= g.size(); guess++)
      {
        private_g[row][col] = guess;
        if (private_g.is_possible())
        {
          if (recur_helper(private_g))
          {
            g = private_g;
            found_solution = true;
            #pragma omp cancel parallel
          }
        }
      }
    }
    return found_solution;

  }

public:
  BackSolve(){
    std::cout << "DFS solver is selected" << std::endl;
  }
  Grid seq_solve(Grid g) override
  {
    // std::vector<Coordinate> coords = g.find_all_empty_cells();
    recur_helper(g);
    return g;
  }

  Grid par_solve(Grid g) override
  {
    recur_helper(g);
    return g;
  }
};

#endif