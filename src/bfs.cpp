#include <vector>
#include <deque>

#include "grid.h"

class BFS
{
public:
    static Grid seq_solve(Grid g)
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

    static Grid par_solve(Grid g)
    {
        std::vector<Coordinate> coords = g.find_all_empty_cells();

        std::deque<Grid> possible;

        possible.push_back(g);
        dtype grid_size = g.size();

        // we will do the bfs solving by choosing one of the unsolved squres
        // then we will find all possibilies for that square
        // at any particular timestep, the coords is all the same
        // pick and solve exactly one square in every full iteration of the while loop
        while (!coords.empty())
        {
            Coordinate c = coords.back();
            coords.pop_back();
            // next_iter.clear();

#pragma omp parallel default(none) shared(c, possible, grid_size)
            {
                std::deque<Grid> next_iter;
                int tid = omp_get_thread_num();
                Grid private_possible;

                while (true)
                {
#pragma omp critical
                    // #pragma omp critical(deque_front)
                    {
                        if (!possible.empty())
                        {
                            private_possible = possible.front();
                            possible.pop_front();
                            // printf("tid %d obtain private:, %s, now possible_cnt %d\n", tid, private_possible.display_values_inline().c_str(), possible.size());
                        }
                        else
                        {
                            private_possible = Grid();
                        }
                    }

                    if (private_possible.size() == 0)
                    {
                        break;
                    }
                    for (dtype guess = 1; guess <= grid_size; guess++)
                    {
                        // overriding value here (should be ok)
                        private_possible[c.r][c.c] = guess;

                        // this function is not efficient and should be replaced
                        // Instead this function will be replaced by taking the current loc
                        // the row/col/and box of the location of the guess and check if its possible
                        if (private_possible.is_possible())
                        {
                            next_iter.push_back(private_possible);
                        }
                    }
                    // printf("tid %d next_iter_cnt:, %d\n", tid, next_iter.size());
                }
#pragma omp barrier

#pragma omp critical
                // #pragma omp critical(deque_end)
                {
                    // possible.insert(possible.front(), next_iter.begin(), next_iter.end());
                    if (!next_iter.empty())
                    {
                        // printf("tid %d before insert next_iter, possible_cnt:, %d\n", tid, possible.size());
                        if (possible.empty())
                        {
                            // possible.insert(possible.front(), next_iter.begin(), next_iter.end());
                            possible = std::deque<Grid>(std::make_move_iterator(next_iter.begin()), std::make_move_iterator(next_iter.end()));
                        }
                        else
                        {
                            possible.insert(possible.end(), next_iter.begin(), next_iter.end());
                        }
                        // printf("tid %d after insert next_iter, possible_cnt:, %d, %s\n", tid, possible.size(), possible[0].display_values_inline().c_str(), possible.size());
                    }
                    // printf("hi\n");
                }

                // exit(1);
            }
        }

        if (!possible.empty())
        {
            return possible[0];
        }

        return Grid();
    }
};
