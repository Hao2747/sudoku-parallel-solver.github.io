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
        bool found_solution = false;
        omp_set_num_threads(4);
        grid_queue.push_back(g);
        omp_lock_t queue_lock;
        omp_init_lock(&queue_lock);
int next_ticket = 0; // next ticket number to be issued
            int now_serving = 0; // current ticket being served
// g.display_values();
#pragma omp parallel shared(g, grid_size, grid_queue, found_solution, now_serving, next_ticket) private(private_g, row, col, possible_grids, get_grid)
        {
            int tid = omp_get_thread_num();
            int nid = omp_get_num_threads();
            


            
            while (!found_solution)
            {
                // std::cout << "tid " << tid << ": still running" << std::endl;

                get_grid = false;
                // #pragma omp cancellation point parallel

                while (tid != now_serving % nid || grid_queue.empty())
                {
                    
#pragma omp atomic read
                    now_serving = now_serving; // force a memory read to update value
                                               // #pragma omp barrier
                                                                        // wait for other threads
                    if (found_solution){
                        break;
                    }
                }
                omp_set_lock(&queue_lock);
                // std::cout << "tid " << tid << ": acquire the lock" << std::endl;
                if (!grid_queue.empty())
                {
                    private_g = grid_queue.front();
                    grid_queue.pop_front();
                    get_grid = true;
                    // std::cout << "tid " << tid << ": get from queue" << std::endl;
                    // private_g.display_values();
                    // std::cout << "only " << grid_queue.size() << " puzzles left in the queue" << std::endl;
                }
                //TODO: take it out of lock
                #pragma omp atomic update
                now_serving++;
                #pragma omp flush(now_serving)
                omp_unset_lock(&queue_lock);

                // std::cout << "tid " << tid << ": release the lock" << std::endl;

                if (!get_grid)
                {
                    continue; // could use while loop to wait until
                }

                // if there is still empty cell, set row and col
                if (!private_g.find_next_empty_cell(row, col))
                {
                    if (!found_solution)
                    {
#pragma omp critical
                        {
                            // if (grid_queue.empty())
                            // {
                            g = private_g;
                            // TODO: omp cancel
                            // #pragma omp cancel parallel
                            // std::cout << "solution found!!!!" << std::endl;
                            found_solution = true;
                            #pragma omp flush(found_solution)
                        }
                    }
                    break;

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
                    omp_set_lock(&queue_lock);

                    {
                        grid_queue.insert(grid_queue.end(), possible_grids.begin(), possible_grids.end());
                    }
                    omp_unset_lock(&queue_lock);
                }
                // std::cout << "tid " << tid << ": now serving: " << now_serving << "grid in queue" << grid_queue.size() << std::endl;

                // for (auto &grid : grid_queue)
                // {
                // grid.display_values();
                // Do something with the current element (stored in elem)
                // }
                // exit(1);

                possible_grids.clear();
            }
        }
        omp_destroy_lock(&queue_lock);
        return g;
    }
};

#endif