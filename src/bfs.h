#include <vector>
#include <deque>
#include <algorithm> 

#include "grid.h"
#include "solver.h"

#ifndef BFS_H
#define BFS_H
class BFS : public Solver
{
public:
    BFS(){
    std::cout << "BFS solver is selected" << std::endl;
  }
    Grid seq_solve(Grid g) override
    {
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        std::deque<Grid> possible;
        std::deque<Grid> next_iter;

        possible.push_back(g);

        // we will do the bfs solving by choosing one of the unsolved squres
        // then we will find all possibilies for that square
        // at any particular timestep, the coords is all the same
        // pick and solve exactly one square in every full iteration of the while loop
        while (!coords.empty())
        {
            Coordinate c = coords.back();
            coords.pop_back();
            next_iter.clear();

            while (!possible.empty())
            {
                Grid private_possible = possible.front();
                possible.pop_front();

                for (dtype guess = 1; guess <= g.size(); guess++)
                {
                    // overriding value here (should be ok)
                    private_possible[c.r][c.c] = guess;

                    // this function is not efficient and should be replaced
                    // Instead this function will be replaced by taking the private_possible loc
                    // the row/col/and box of the location of the guess and check if its possible
                    if (private_possible.is_possible())
                    {
                        next_iter.push_back(private_possible);
                    }
                }
            }

            possible.swap(next_iter);
        }

        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }

     Grid par_solve(Grid g) override
    {
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        std::deque<Grid> possible;

        possible.push_back(g);
        dtype grid_size = g.size();

        // we will do the bfs solving by choosing one of the unsolved squres
        // then we will find all possibilies for that square
        // at any particular timestep, the coords is all the same
        // pick and solve exactly one square in every full iteration of the while loop
            std::deque<Grid> next_iter;

        while (!coords.empty())
        {
            Coordinate c = coords.back();
            coords.pop_back();
            next_iter.clear();

            
            int possible_size = possible.size();
            next_iter.resize(possible_size * 9);
#pragma omp parallel for
            for (int i = 0; i < possible_size; i++)
           
            {
                int tid = omp_get_thread_num();
                Grid private_possible;
                private_possible = possible[i];
                
                for (dtype guess = 1; guess <= grid_size; guess++)
                {
                    // overriding value here (should be ok)
                    private_possible[c.r][c.c] = guess;

                    // this function is not efficient and should be replaced
                    // Instead this function will be replaced by taking the current loc
                    // the row/col/and box of the location of the guess and check if its possible
                    if (private_possible.is_possible())
                    {
                        next_iter[tid*9+(guess-1)] =private_possible;
                    }
                }

            }

            next_iter.erase(std::remove_if(next_iter.begin(), next_iter.end(), [](Grid i){return i.empty() ;}), next_iter.end());
            possible.swap(next_iter);
        }

        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }
};


#endif