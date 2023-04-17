#include <vector>
#include <deque>

#include "grid.h"

class BFS {
  public:
  static Grid seq_solve(Grid g) {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    
    std::deque<Grid> possible;
    std::deque<Grid> next_iter;
    
    possible.push_back(g);
    
    //we will do the bfs solving by choosing one of the unsolved squres
    //then we will find all possibilies for that square
    //at any particular timestep, the coords is all the same
    //pick and solve exactly one square in every full iteration of the while loop
    while(!coords.empty()) {
        Coordinate c = coords.back();
        coords.pop_back();
        next_iter.clear();
        
        while(!possible.empty()) {
            Grid current = possible.front();
            possible.pop_front();
            
            for(dtype guess = 1; guess <= g.size(); guess++) {
                //overriding value here (should be ok)
                current[c.r][c.c] = guess;
                
                //this function is not efficient and should be replaced
                //Instead this function will be replaced by taking the current loc
                //the row/col/and box of the location of the guess and check if its possible
                if(current.is_possible()) {
                    next_iter.push_back(current);
                }
            }
        }
        
        possible.swap(next_iter);
    }
    
    if(!possible.empty()) {
        return possible[0];
    } 
        
    return Grid();
    
  }
  
  static Grid par_solve(Grid g) {
      
     std::vector<Coordinate> coords = g.find_all_empty_cells_parallel();
    
    std::deque<Grid> possible;
    std::deque<Grid> next_iter;
    
    possible.push_back(g);
    
    //we will do the bfs solving by choosing one of the unsolved squres
    //then we will find all possibilies for that square
    //at any particular timestep, the coords is all the same
    //pick and solve exactly one square in every full iteration of the while loop
    while(!coords.empty()) {
        Coordinate c = coords.back();
        coords.pop_back();
        next_iter.clear();
        
        while(!possible.empty()) {
            Grid current = possible.front();
            possible.pop_front();
            
            for(dtype guess = 1; guess <= g.size(); guess++) {
                //overriding value here (should be ok)
                current[c.r][c.c] = guess;
                
                //this function is not efficient and should be replaced
                //Instead this function will be replaced by taking the current loc
                //the row/col/and box of the location of the guess and check if its possible
                if(current.is_possible()) {
                    next_iter.push_back(current);
                }
            }
        }
        
        possible.swap(next_iter);
    }
    
    if(!possible.empty()) {
        return possible[0];
    } 
        
    return Grid();
    
  }
    
};