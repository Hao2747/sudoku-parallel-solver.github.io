#include <vector>

#include "grid.h"
#include "solver.h"

#ifndef CP_NO_BARRIER_H
#define CP_NO_BARRIER_H
// Constrain propagation
class CP_No_Barrier : public Solver
{
private:
    //Return a vector showing how many choices avaiable in each empty cell 
    std::vector<int> get_empty_cell_choices_size(Grid &g, std::vector<Coordinate> &coords)
    {
        std::vector<int> empty_cell_choices_size;
        for (auto coord : coords)
        {
            std::cout << "\nCoord: " << coord.r << "," << coord.c << ": ";
            empty_cell_choices_size.emplace_back(g[coord.r][coord.c].get_choices_cnt());
        }
        return empty_cell_choices_size;
    }

    std::vector<int> get_cumulative_product_choices_size(std::vector<int> &empty_cell_choices_size){
        int size = empty_cell_choices_size.size();
        std::vector<int> cumulative_product_choices_size(size);
        cumulative_product_choices_size[0] = empty_cell_choices_size[0];
        for (int i = 1; i < size; i++)
        {
            cumulative_product_choices_size[i] = cumulative_product_choices_size[i - 1] * empty_cell_choices_size[i];
        }
        return cumulative_product_choices_size;
    }

    std::vector<int> get_tid_to_choice_lookup(int tid, std::vector<int> &cumulative_product_choices_size, std::vector<int> &empty_cell_choices_size){
        int size = cumulative_product_choices_size.size();
        std::vector<int> tid_to_choice_lookup(size);
        tid_to_choice_lookup[0] = tid % cumulative_product_choices_size[0];
        for (int i = 1; i < size; i++)
        {
            tid_to_choice_lookup[i] = (tid / cumulative_product_choices_size[i-1]) % empty_cell_choices_size[i];
        }
        return tid_to_choice_lookup;
    }

public:
    CP_No_Barrier()
    {
        std::cout << "CP_No_Barrier solver is selected" << std::endl;
    }
    Grid seq_solve(Grid g) override
    {
        g.display_values();
        std::vector<Coordinate> coords = g.find_all_empty_cells();
        g.set_square_choices(coords);
        std::vector<int> empty_cell_choice_size = get_empty_cell_choices_size(g, coords);
        std::vector<int> cumulative_product_choices_size = get_cumulative_product_choices_size(empty_cell_choice_size);
        for (int i = 0; i < empty_cell_choice_size.size(); i++) {
            std::cout << empty_cell_choice_size[i] << " ";
        }
        std::cout << "\nCumulative" << std::endl;
        for (int i = 0; i < cumulative_product_choices_size.size(); i++) {
            std::cout << cumulative_product_choices_size[i] << " ";
        }
        std::cout << std::endl;
        std::cout << "\nLookup" << std::endl;
        for (int tid = 0; tid <10; tid++){
        std::vector<int> tid_to_choice_lookup = get_tid_to_choice_lookup(tid, cumulative_product_choices_size,empty_cell_choice_size);
        std::cout << tid << ": " ;
        for (int i = 0; i < tid_to_choice_lookup.size(); i++) {
            std::cout << tid_to_choice_lookup[i] << " ";
        }
        std::cout << std::endl;
        }
        return g;
    }

    Grid
    par_solve(Grid g) override
    {
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        return g;
    }
};

#endif