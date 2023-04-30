#include <vector>

#include "grid.h"
#include "solver.h"

#ifndef CP_BACKSOLVE_H
#define CP_BACKSOLVE_H
// Constrain propagation
class CP_Backsolve : public Solver
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

    std::vector<int> available_choices = g[row][col].get_available_choices(); // Initialize an empty vector to store indices of 1 bits
    

    Grid private_g = g;
    for (dtype guess : available_choices)
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

      std::vector<int> available_choices; // Initialize an empty vector to store indices of 1 bits
      std::bitset<BIT_CNT> square_choices = g[row][col].get_choices();
      // Traverse all bits where the bit is set to 1
      for (int i = 0; i < square_choices.size(); i++)
      {
        if (square_choices.test(i))
        {
          available_choices.push_back(i + 1); // Add the index to the vector
        }
      }

      Grid private_g = g;
      // #pragma omp cancellation point parallel
      for (dtype guess : available_choices)
      {
        private_g[row][col] = guess;
        if (private_g.is_possible())
        {
          if (recur_helper(private_g))
          {
            g = private_g;
            found_solution = true;
            // #pragma omp cancel parallel
          }
        }
      }
    }
    return found_solution;
  }

public:
  CP_Backsolve()
  {
    std::cout << "CP_DFS solver is selected" << std::endl;
  }
  Grid seq_solve(Grid g) override
  {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    g.set_square_choices(coords);
    recur_helper(g);
    return g;
  }

  Grid par_solve(Grid g) override
  {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    g.set_square_choices(coords);
    recur_helper(g);
    return g;
  }
};

#endif