#include <vector>
#include <deque>
#include <algorithm> 

#include "grid.h"
#include "solver.h"
#include "timing.h"
#include "bfs.h"

#ifndef CPHYBRID_H
#define CPHYBRID_H
class CP_Hybrid : public BFS
{
public:
  CP_Hybrid(){
    std::cout << "Hybrid Solver is selected" << std::endl;
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
        //Timer t;
        g.seq_init_choices();
        //std::cout << t.elapsed() << std::endl;
        
        std::deque<Grid> possible;
        std::deque<Grid> next_iter;
        int num_cores = omp_get_num_procs();

        double RATIO = 1.2;

        
        possible.push_back(g);

        while (possible[0].get_coords().size() != 0)
        {
          next_iter.clear();
          if(possible.size() < (size_t)(RATIO * num_cores)) {
            #pragma omp parallel for schedule(dynamic) shared(possible) reduction(merge_grids : next_iter)
            for(int left = 0; left < possible.size(); left++ )
            { 
              std::deque<Grid> private_iter;
              Grid private_possible = possible[left];
              
              Coordinate curr = private_possible.pop_coord();

              int row = curr.r;
              int col = curr.c; 

              std::set<dtype> available_choices = private_possible.find_choices_from_coord(row, col);

              if(available_choices.size() != 0) {
                for (dtype guess : available_choices)
                {
                  private_possible[row][col] = guess;
                  private_possible.update_annotations(row,col, guess);
                  
                  private_iter.push_back(private_possible);

                }
                
                next_iter.insert(next_iter.end(), private_iter.begin(), private_iter.end());
              }
            }

            possible.swap(next_iter);
          } else {
            std::cout<<"(never reaches here)using all threads" << std::endl;
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
                }
              }
              
              #pragma omp cancellation point for
            }
            
            if(ans.empty()) {
              std::cout << "found nothing" << std::endl;
            }

            return ans;
          }
        }

        std::cout << "finished" << std::endl;
        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }
};


#endif