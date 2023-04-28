#include <vector>

#include "grid.h"
#include "solver.h"

#ifndef CP_H
#define CP_H
//Constrain propagation
class CP : public Solver
{

public:
    CP()
    {
        std::cout << "CP solver is selected" << std::endl;
    }
    Grid seq_solve(Grid g) override
    {
        // std::vector<Coordinate> coords = g.find_all_empty_cells();
        // size_t grid_size = g.size();
        // int row[grid_size];
        g.set_square_choices();
        g.print_arrays();
        return g;
    }

    Grid par_solve(Grid g) override
    {
        // recur_helper(g);
        return g;
    }
};



#endif