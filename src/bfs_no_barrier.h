#include <vector>
#include <deque>
#include <algorithm>

#include "grid.h"
#include "solver.h"

#ifndef BFS_NO_BARRIER_H
#define BFS_NO_BARRIER_H
class BFS_NO_BARRIER : public Solver
{
public:
    BFS_NO_BARRIER()
    {
        std::cout << "BFS_NO_BARRIER solver is selected" << std::endl;
    }
    Grid seq_solve(Grid g) override
    {
        Grid private_g;
        std::deque<Grid> grid_queue;
        int row, col;
        dtype guess;
        std::deque<Grid> possible_grids;
        int grid_size = g.size();

        grid_queue.push_back(g);
        // g.display_values();
        while (!grid_queue.empty())
        {
            private_g = grid_queue.front();
            grid_queue.pop_front();

            // if there is still empty cell, set row and col
            if (!private_g.find_next_empty_cell(row, col))
            {
                g = private_g;
                break;
            }
            // std::cout << row << col << std::endl;
            // make guesses, if it is possible, added to the queue
            for (guess = 1; guess <= grid_size; guess++)
            {
                private_g[row][col] = guess;
                // private_g.display_values();
                if (private_g.is_possible())
                {
                    possible_grids.emplace_back(private_g);
                }
            }
            if (!possible_grids.empty())
            {
                grid_queue.insert(grid_queue.end(), possible_grids.begin(), possible_grids.end());
            }

            // for (auto &grid : grid_queue)
            // {
            // grid.display_values();
            // Do something with the current element (stored in elem)
            // }
            // exit(1);

            possible_grids.clear();
        }
        return g;
    }

    Grid par_solve(Grid g) override
    {
        Grid private_g;
        std::deque<Grid> grid_queue;
        int row, col;
        dtype guess;
        std::deque<Grid> possible_grids;
        int grid_size = g.size();
        bool get_grid;

        grid_queue.push_back(g);
// g.display_values();
#pragma omp parallel shared(g, grid_size) private(private_g, row, col, possible_grids, get_grid)
        {
            int tid = omp_get_thread_num();
            while (true)
            {
      #pragma omp cancellation point parallel

                get_grid = false;
#pragma omp critical
                {
                    if (!grid_queue.empty())
                    {
                        private_g = grid_queue.front();
                        grid_queue.pop_front();
                        get_grid = true;
                        std::cout << "tid " << tid << ": get" << std::endl;
                        private_g.display_values();
                        std::cout << "only " << grid_queue.size() << " puzzles left in the queue" << std::endl;
                    }
                }

                if (!get_grid)
                {
                    continue;
                }

                // if there is still empty cell, set row and col
                if (!private_g.find_next_empty_cell(row, col))
                {
                    // #pragma omp critical
                    //                     {
                    if (grid_queue.empty())
                    {
                        g = private_g;
                        // TODO: omp cancel
            #pragma omp cancel parallel
                        std::cout << "solution found!!!!" << std::endl;
                        break;
                    }
                    // }
                }
                // std::cout << row << col << std::endl;
                // make guesses, if it is possible, added to the queue
                for (guess = 1; guess <= grid_size; guess++)
                {
                    private_g[row][col] = guess;
                    // private_g.display_values();
                    if (private_g.is_possible())
                    {
                        possible_grids.emplace_back(private_g);
                    }
                }
                if (!possible_grids.empty())
                {
#pragma omp critical
                    {
                        grid_queue.insert(grid_queue.end(), possible_grids.begin(), possible_grids.end());
                    }
                }

                // for (auto &grid : grid_queue)
                // {
                // grid.display_values();
                // Do something with the current element (stored in elem)
                // }
                // exit(1);

                possible_grids.clear();
            }
        }
        return g;
    }
};

#endif