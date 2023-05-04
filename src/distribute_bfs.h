#include <vector>
#include <deque>
#include <algorithm> 

#include "grid.h"
#include "solver.h"
#include "timing.h"
#include "bfs.h"

#ifndef DISTRIBUTEBFS_H
#define DISTRIBUTEBFS_H
class DistributeBFS : public BFS
{
public:
  DistributeBFS(){
    std::cout << "Distribute BFS solver is selected" << std::endl;
  }


  bool inline recur_helper(Grid &g, bool& found, Grid &ans)
  {

    int row, col;

    // If cannot find more empty cell, puzzle is solved
    // If can find, set row and col of first empty cell
    
    if(found) {
      return false;
    }
    if (!g.find_next_empty_cell(row, col))
    {
      return true;
    }

    Grid private_g = g;
    for (dtype guess = 1; guess <= g.size(); guess++)
    {
          if(found) {
      return false;
    }
      private_g[row][col] = guess;
      
      if (private_g.is_possible())
      {
            if(found) {
      return false;
    }
        if (recur_helper(private_g, found, ans))
        {
          g = private_g;
          return true;
        }
            if(found) {
      return false;
    }
      }
    }

    return false;
  }
     Grid par_solve(Grid g) override
    {
        #pragma omp declare reduction (merge_grids :std::deque<Grid>: omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        std::deque<Grid> possible;
        std::deque<Grid> next_iter;
        int num_cores = omp_get_num_procs();

        double RATIO = 1.5;

        
        possible.push_back(g);

        while (!coords.empty())
        {
          Coordinate c = coords.back();
          coords.pop_back();
          next_iter.clear();

          //std::cout << possible.size() << std::endl;
          if(possible.size() < (size_t)(RATIO * num_cores)) {
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
          } else {
            bool found = false;
            Grid ans;
            
            //Timer t;
            //#pragma omp taskgroup 
            #pragma omp parallel for schedule(dynamic) shared(possible, found, ans)
            for(int left = 0; left < possible.size(); left++ )
            { 
              #pragma omp cancellation point for
              Grid private_possible = possible[left];

              if(recur_helper(private_possible, found, ans)) {
                if(!found) {
                  ans = private_possible;
                  found = true;
                  
                  
                  #pragma omp cancel for
                
                  //std::cout << t.elapsed() << std::endl;
                }
              }
              
              #pragma omp cancellation point for
            }

            //std::cout << t.elapsed() << std::endl;
            return ans;
          }
        }

        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }
};


#endif