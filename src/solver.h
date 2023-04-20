#include "grid.h"

#ifndef SOLVER_H
#define SOLVER_H


class Solver {
public:
  virtual Grid seq_solve(Grid g) = 0;
  virtual Grid par_solve(Grid g) = 0;
  
};

#endif