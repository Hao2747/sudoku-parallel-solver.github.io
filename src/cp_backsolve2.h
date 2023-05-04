#include <vector>
#include <atomic> 

#include "grid.h"
#include "solver.h"
#include "bfs.h"
#include "timing.h"

#ifndef CP_BACKSOLVE2_H
#define CP_BACKSOLVE2_H
// Constrain propagation
class CP_Backsolve2 : public CP_Backsolve
{
private:
  std::atomic<bool> found ;
  Grid ans;

  Grid seq_recur_helper(Grid& g) {

    if(g.get_coords().size() == 0) { //if gotten here grid is always correct and valid 
      return g;
    }
    
    Coordinate curr = g.pop_coord();
    int row = curr.r;
    int col = curr.c; 
    std::set<dtype> available_choices = g.find_choices_from_coord(row, col);
    
    if(available_choices.size() == 0) {
      return Grid();
    } else {
      for (dtype guess : available_choices){

        {
          Grid private_g = g;
          private_g[row][col] = guess;
          private_g.update_annotations(row,col, guess);

          private_g = seq_recur_helper(private_g);
        
          if(!private_g.empty()) {
            return private_g;
          }

        }

      }
    }
  
    return ans;
    
  }

  
  
  Grid new_par_recur_helper(Grid g, int depth) {
    if(found) {
      return ans; 
    }
    if(g.get_coords().size() == 0) { //if gotten here grid is always correct and valid 
      return g;
    }
    
    Coordinate curr = g.pop_coord();
    int row = curr.r;
    int col = curr.c; 
    std::set<dtype> available_choices = g.find_choices_from_coord(row, col);

    if(available_choices.size() == 0) {
      return Grid();
    } else {
      for (dtype guess : available_choices){
        if(found) {
          return ans; 
        }

        if(depth <=4) {
        
        #pragma omp task shared(found, ans) untied 
        {
          Grid private_g = g;
          private_g[row][col] = guess;
          
          private_g.update_annotations(row,col, guess);
          
          
          private_g = new_par_recur_helper(private_g, depth+1);
          if(!found) {
            if(!private_g.empty()) {
              ans = private_g;
              found = true;
              
              std::cout <<"answer found" << std::endl;
              #pragma omp cancel taskgroup
            }
          }
        }
        } else {
          Grid private_g2 = g;
          private_g2[row][col] = guess;
          
          private_g2.update_annotations(row,col, guess);

          private_g2 = seq_recur_helper(private_g2);
          if(!private_g2.empty()) {
            ans = private_g2;
            found = true;
          }

        }
      }
      
      #pragma omp taskwait
      return ans;
    }
    std::cout <<"answer found" << std::endl;
    return ans;
    
  }
public:
  CP_Backsolve2()
  {
    std::cout << "CP_BackSolve Version 2 Overlap Guesser solver is selected" << std::endl;
    found = false;
    ans = Grid();
  }

  Grid par_solve(Grid g) override
  {
    
    g.seq_init_choices();
    Grid a;
    //recur_helper(g);
    #pragma omp parallel shared(found, ans)
    {
      #pragma omp single 
      {  
        #pragma omp taskgroup 
        {
          a = new_par_recur_helper(g, 0);
        }
      }
    }
    return a;
  }
};

#endif