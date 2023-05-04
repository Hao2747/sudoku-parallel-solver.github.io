#include <vector>


#include "grid.h"
#include "solver.h"
#include "bfs.h"

#ifndef CP_NO_BARRIER_H
#define CP_NO_BARRIER_H
// Constrain propagation
class CP_No_Barrier : public BFS
{
private:
    // Return a vector showing how many choices avaiable in each empty cell
    std::vector<int> get_empty_cell_choices_size(Grid &g, std::vector<Coordinate> &coords)
    {
        int empty_cell_cnt = coords.size();
        std::vector<int> empty_cell_choices_size(empty_cell_cnt);
        for (int i = 0; i < empty_cell_cnt; i++)
        {
            Coordinate coord = coords[i];
            // std::cout << "\nCoord: " << coord.r << "," << coord.c << ": " << g[coord.r][coord.c].get_choices();
            empty_cell_choices_size[i] = (g[coord.r][coord.c].get_choices_cnt());
            
        }

        return empty_cell_choices_size;
    }

    std::vector<int> get_cumulative_product_choices_size(std::vector<int> &empty_cell_choices_size)
    {
        int size = empty_cell_choices_size.size();
        std::vector<int> cumulative_product_choices_size(size);
        cumulative_product_choices_size[0] = empty_cell_choices_size[0];
        for (int i = 1; i < size; i++)
        {
            cumulative_product_choices_size[i] = cumulative_product_choices_size[i - 1] * empty_cell_choices_size[i];
        }
        return cumulative_product_choices_size;
    }

    std::vector<int> get_personal_look_up_index(int tid, std::vector<int> &cumulative_product_choices_size, std::vector<int> &empty_cell_choices_size)
    {
        int size = cumulative_product_choices_size.size();
        std::vector<int> personal_look_up_index(size);
        personal_look_up_index[0] = tid % cumulative_product_choices_size[0];
        for (int i = 1; i < size; i++)
        {
            personal_look_up_index[i] = (tid / cumulative_product_choices_size[i - 1]) % empty_cell_choices_size[i];
        }
        return personal_look_up_index;
    }

public:
    CP_No_Barrier()
    {
        std::cout << "CP_No_Barrier solver is selected" << std::endl;
    }
    // Grid seq_solve(Grid g) override
    // {
        // g.display_values();

        // std::vector<Coordinate> coords = g.find_all_empty_cells();

        // Coordinate empty_cell_coord;
        // dtype guess;
        // int choice_index;
        // bool found_solution;
        // g.set_square_choices(coords);
        // std::vector<int> empty_cell_choice_size = get_empty_cell_choices_size(g, coords);
        // std::vector<int> cumulative_product_choices_size = get_cumulative_product_choices_size(empty_cell_choice_size);

        // int total_num_choices = cumulative_product_choices_size[cumulative_product_choices_size.size()-1];

        
        // for (int tid = 0; tid < total_num_choices; tid++)
        // {
            // found_solution = true;
            // Grid private_g = g;
            // std::vector<int> personal_look_up_index = get_personal_look_up_index(tid, cumulative_product_choices_size, empty_cell_choice_size);

            // for (int empty_cell_index = 0; empty_cell_index < empty_cell_choice_size.size(); empty_cell_index++)
            // {
                // empty_cell_coord = coords[empty_cell_index];
                // choice_index = personal_look_up_index[empty_cell_index];
                // guess = private_g[empty_cell_coord.r][empty_cell_coord.c].get_available_choices()[choice_index];
                // private_g[empty_cell_coord.r][empty_cell_coord.c] = guess;
                // if (!private_g.is_possible()){
                    // found_solution = false;
                    // break;
                // }
            // }
            // if (found_solution){
                // return private_g;
            // }
        // }
        // return g;
    // }

    Grid
    par_solve(Grid g) override
    {
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        return g;
    }
};

#endif