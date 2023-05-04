#include <vector>
#include <atomic> 

#include "grid.h"
#include "solver.h"
#include "bfs.h"
#include "timing.h"

#ifndef CP_BACKSOLVE_H
#define CP_BACKSOLVE_H
// Constrain propagation
class CP_Backsolve : public BFS
{
private:
  std::atomic<bool> found ;
  Grid ans;

  bool recur_helper(Grid &g)
  {
    // g.display_values();
    // int row, col;

    // // If cannot find more empty cell, puzzle is solved
    // // If can find, set row and col of first empty cell
    // if (!g.find_next_empty_cell(row, col))
    // {
      // return true;
    // }

    // std::vector<int> available_choices = g[row][col].get_available_choices(); // Initialize an empty vector to store indices of 1 bits
    

    // Grid private_g = g;
    // for (dtype guess : available_choices)
    // {
      // private_g[row][col] = guess;
      // if (private_g.is_possible())
      // {
        // if (recur_helper(private_g))
        // {
          // g = private_g;
          // return true;
        // }
      // }
    // }

    return false;
  }
  bool recur_helper_par(Grid &g)
  {
    // int row, col;

    // // If cannot find more empty cell, puzzle is solved
    // // If can find, set row and col of first empty cell
    // if (!g.find_next_empty_cell(row, col))
    // {
      // return true;
    // }
    // bool found_solution = false;

// #pragma omp parallel shared(found_solution)
    // {

      // std::vector<int> available_choices; // Initialize an empty vector to store indices of 1 bits
      // std::bitset<BIT_CNT> square_choices = g[row][col].get_choices();
      // // Traverse all bits where the bit is set to 1
      // for (int i = 0; i < square_choices.size(); i++)
      // {
        // if (square_choices.test(i))
        // {
          // available_choices.push_back(i + 1); // Add the index to the vector
        // }
      // }

      // Grid private_g = g;
      // // #pragma omp cancellation point parallel
      // for (dtype guess : available_choices)
      // {
        // private_g[row][col] = guess;
        // if (private_g.is_possible())
        // {
          // if (recur_helper(private_g))
          // {
            // g = private_g;
            // found_solution = true;
            // // #pragma omp cancel parallel
          // }
        // }
      // }
    // }
    //return found_solution;
    return false;
  }

  Grid seq_recur_helper(Grid& g, std::vector<Coordinate> coords) {

    if(coords.size() == 0) { //if gotten here grid is always correct and valid 
      return g;
    }
    
    Coordinate curr = coords.back();
    coords.pop_back();
    int row = curr.r;
    int col = curr.c; 
    
    std::set<dtype> available_choices = g[row][col].get_choices();
    
    if(available_choices.size() == 0) {
      return Grid();
    } else {
      for (dtype guess : available_choices){

        //#pragma omp task shared(found, ans) untied 
        {
          Grid private_g = g;
          private_g[row][col] = guess;
          //update annotations 
          if (private_g.is_possible()) {
            private_g = seq_recur_helper(private_g, coords);
          
            if(!private_g.empty()) {
              return private_g;
            }
          }
        }

      }
    }
  
    return ans;
    
  }


  // Grid new_par_recur_helper(Grid g, std::vector<Coordinate> coords, int depth) {

    // if(found) {
      // return ans; 
    // }
    // if(coords.size() == 0) { //if gotten here grid is always correct and valid 
      // return g;
    // }
    
    // Coordinate curr = coords.back();
    // coords.pop_back();
    // int row = curr.r;
    // int col = curr.c; 

    // std::set<dtype> available_choices = g[row][col].get_choices();
    
    
    // if(available_choices.size() == 0) {
      // return Grid();
    // } else {
      // for (dtype guess : available_choices){
        // if(found) {
          // return ans; 
        // }

        // if(depth <=4) {
        // #pragma omp task shared(found, ans) untied 
        // {
          // Grid private_g = g;
          // private_g[row][col] = guess;
          // //update annotations 
          // if (private_g.is_possible()) {
            // private_g = new_par_recur_helper(private_g, coords, depth+1);
            // if(!private_g.empty()) {
              // ans = private_g;
              // found = true;
            // }
          // }
        // }
        // } else {
          // Grid private_g2 = g;
          // private_g2[row][col] = guess;
          // //update annotations 
          // if (private_g2.is_possible()) {
            // private_g2 = seq_recur_helper( private_g2, coords);
            // if(!private_g2.empty()) {
              // ans = private_g2;
              // found = true;
            // }
          // }

        // }
      // }
    // }
    // return ans;
    
  // }

  // Grid new_par_recur_helper(Grid g, std::vector<Coordinate> coords, int depth) {

    // if(found) {
      // return ans;
    // }
    // if(coords.size() == 0) { //if gotten here grid is always correct and valid 
      // return g;
    // }
    
    // Coordinate curr = coords.back();
    // coords.pop_back();
    // int row = curr.r;
    // int col = curr.c; 
    
    // std::set<dtype> available_choices = g[row][col].get_choices();
    
    // if(available_choices.size() == 0) {
      // return Grid();
    // } else {
      // for (dtype guess : available_choices){

        // if(found) {
          // return ans;
        // }
        // #pragma omp task shared(ans,found) untied 
        // {
          // Grid private_g = g;
          // private_g[row][col] = guess;

          // if (private_g.is_possible()) {
            // private_g = new_par_recur_helper(private_g, coords, depth+1);
          
            // if(!private_g.empty()) {  

              // ans = private_g;

              // //#pragma omp atomic write
              // found = true;

              // std::cout << " found something" << std::endl;
              // #pragma omp cancel taskgroup
              
            // }
          // }
        // }

      // }
      
      // #pragma omp taskwait
      // return ans;
    // }
  
    // return ans;

  // }

/**************************** working iteration 1 *************************/

  Grid new_par_recur_helper1(Grid g, std::vector<Coordinate> coords, int depth) {
    if(found) {
      return ans;
    }

    if(coords.size() == 0) { //if gotten here grid is always correct and valid 
      return g;
    }
    
    Coordinate curr = coords.back();
    coords.pop_back();
    int row = curr.r;
    int col = curr.c; 
    
    std::set<dtype> available_choices = g[row][col].get_choices();
    
    if(available_choices.size() == 0) {
      return Grid();
    } else {
      for (dtype guess : available_choices){
        if(found) {
          return ans;
        }
        #pragma omp task shared(ans,found) untied 
        {
          Grid private_g = g;
          private_g[row][col] = guess;

          if (private_g.is_possible()) {
            private_g = new_par_recur_helper(private_g, coords, depth+1);
          
            if(!found) {
              if(!private_g.empty()) {  
                  ans = private_g;

                  //#pragma omp atomic write
                  found = true;
                //std::cout << " found something" << std::endl;
                #pragma omp cancel taskgroup
                
              }
            }
          }
        }

      }
      
      #pragma omp taskwait
      return ans;
    }
  
    return ans;

  }
  
  
  Grid new_par_recur_helper(Grid g, std::vector<Coordinate> coords, int depth) {
    if(found) {
      return ans; 
    }
    if(coords.size() == 0) { //if gotten here grid is always correct and valid 
      return g;
    }
    
    Coordinate curr = coords.back();
    coords.pop_back();
    int row = curr.r;
    int col = curr.c; 

    std::set<dtype> available_choices = g[row][col].get_choices();
    
    
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
          //update annotations 
          if (private_g.is_possible()) {
            private_g = new_par_recur_helper(private_g, coords, depth+1);
            if(!found) {
              if(!private_g.empty()) {
                ans = private_g;
                found = true;
                
                #pragma omp cancel taskgroup
              }
            }
          }
        }
        } else {
          Grid private_g2 = g;
          private_g2[row][col] = guess;
          //update annotations 
          if (private_g2.is_possible()) {
            private_g2 = seq_recur_helper(private_g2, coords);
            if(!private_g2.empty()) {
              ans = private_g2;
              found = true;
              //TODO: much later ... weird bug have to debug later
              //return ans;
            }
          }

        }
      }
      
      #pragma omp taskwait
      return ans;
    }
    return ans;
    
  }
public:
  CP_Backsolve()
  {
    std::cout << "CP_BFS Overlap Guesser solver is selected" << std::endl;
    found = false;
    ans = Grid();
  }

  Grid par_solve(Grid g) override
  {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    g.set_square_choices(coords);
    
    Grid a;
    //recur_helper(g);
    #pragma omp parallel shared(found, ans)
    {
      #pragma omp single 
      {  
        #pragma omp taskgroup 
        {
          a = new_par_recur_helper(g, coords, 0);
        }
      }
    }
    return a;
  }
};

#endif