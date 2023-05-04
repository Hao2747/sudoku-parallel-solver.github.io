#include <vector>
#include <deque>
#include <algorithm> 

#include "grid.h"
#include "solver.h"
#include "bfs.h"

#ifndef DISTRIBUTEBFS_H
#define DISTRIBUTEBFS_H
class DistributeBFS : public BFS
{
public:
  DistributeBFS(){
    std::cout << "Distribute BFS solver is selected" << std::endl;
  }

     Grid par_solve(Grid g) override
    {
        #pragma omp declare reduction (merge_grids :std::deque<Grid>: omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        std::deque<Grid> possible;
        std::deque<Grid> next_iter;
        int num_cores = omp_get_num_procs();

        int RATIO = 3;

        
        possible.push_back(g);

        while (!coords.empty())
        {
          Coordinate c = coords.back();
          coords.pop_back();
          next_iter.clear();

        
        
          #pragma omp parallel for schedule(dynamic) shared(possible) reduction(merge_grids : next_iter)
          for(int left = 0; left < possible.size(); left++ )
          { 
            std::deque<Grid> private_iter;
            Grid private_possible = possible[left];

            for (dtype guess = 1; guess <= g.size(); guess++)
            {
              private_possible[c.r][c.c] = guess;
              
              if (private_possible.is_possible())
              {
                  private_iter.push_back(private_possible);
              }
            }
            
            next_iter.insert(next_iter.end(), private_iter.begin(), private_iter.end());
          }
          possible.swap(next_iter);
        }

        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }
};


#endif