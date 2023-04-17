#include <vector>

#include "grid.h"


class BackSolve {
private:
  static Grid recur_helper(Grid g, std::vector<Coordinate> coords) {
    if(coords.empty()) {
     if(g.validate()) {
            return g;
        } else {
            return Grid();
        }
    }
    
    Coordinate c = coords.back();
    coords.pop_back();

    for(dtype guess = 1; guess <= g.size(); guess++) {
        g[c.r][c.c] = guess;
        Grid ret = recur_helper(g, coords);
        
        if(!ret.empty()) {
            return ret;
        }
    }
    return Grid();
  }
public:
  static Grid seq_solve(Grid g) {
    std::vector<Coordinate> coords = g.find_all_empty_cells();
    
    return recur_helper(g, coords);
  }
  static Grid par_solve(Grid g) {
    
    //implement this later after BFS for checkpoint
    return Grid();
  }
};