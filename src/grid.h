#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>
#include <climits>

#include <cmath>
#include <cassert>
#include <omp.h>
#include <set>
#include <memory>
#include <bitset>

#ifndef GRID_H
#define GRID_H

typedef int dtype;

static int UNASSIGNED = 0;
static size_t const BIT_CNT = 4;

struct Coordinate
{
    size_t r;
    size_t c;

    
};


typedef struct Coordinate Coordinate;

class Square
{
private:
    dtype num;
    std::bitset<BIT_CNT> choices;

public:
    Square() : num(UNASSIGNED) {}
    Square(dtype val) : num(val) {}

    bool is_solved() { return (num == UNASSIGNED) ? false : true; };
    dtype &value()
    {
        return num;
    }

    // override = operator, so can assign value directly
    void operator=(const dtype num)
    {
        this->num = num;
    }

    void set_choices(std::bitset<BIT_CNT> &c)
    {
        choices = c;
    }

    std::bitset<BIT_CNT> get_choices()
    {
        return choices;
    }

    int get_choices_cnt(){
        return choices.count();
    }

    std::vector<int> get_available_choices()
    {
        std::vector<int> available_choices; // Initialize an empty vector to store indices of 1 bits

        // Traverse all bits where the bit is set to 1
        for (int i = 0; i < choices.size(); i++)
        {
            if (choices.test(i))
            {
                available_choices.push_back(i + 1); // Add the index to the vector
            }
        }
        return available_choices;
    }
};

class Grid
{
public:
    class GridRow
    {
    private:
        friend class Grid; // Allow Grid to Create & Access Internals
        std::vector<Square> vec;
        GridRow(const size_t size) : vec(size, Square()) {}
        GridRow(const std::vector<dtype> v)
        {
            for (auto s : v)
            {
                vec.emplace_back(Square(s));
            }
        }

    public:
        Square &operator[](size_t idx) { return vec[idx]; }
        size_t size() { return vec.size(); }
        std::vector<dtype> values()
        {
            std::vector<dtype> res;
            for (auto sq : vec)
            {
                res.emplace_back(sq.value());
            }
            return res;
        }
    };

private:
    size_t grid_size; // row & col
    size_t block_len; // sqrt(row)

    std::vector<GridRow> grid;
    std::vector<std::bitset<BIT_CNT>> row;
    std::vector<std::bitset<BIT_CNT>> col;
    std::vector<std::bitset<BIT_CNT>> box;

    // checks is the vectors are possible in sudoku
    // returns : bool - true or false if possible
    //    size_t - number of digits missing in row (only valid when bool is true)
    std::pair<bool, size_t> check_if_vecs_possible(const std::vector<dtype> &vec)
    {
        if (vec.size() != grid_size)
        {
            return std::make_pair(false, 0);
        }

        // use a vector to check if value has been seen
        std::vector<bool> seen(vec.size(), false);
        size_t num_left = grid_size;

        for (auto val : vec)
        {
            if (val <= 0)
            {
                // sudoku numbers are from 1 to gridsize
                continue;
            }

            auto val_index = val - 1;
            if (seen[val_index] == false)
            {
                seen[val_index] = true;
                num_left--;
            }
            else
            {
                // seen more than once
                return std::make_pair(false, 0);
            }
        }

        return std::make_pair(true, num_left);
    }
    bool is_possible(const std::vector<dtype> &vec)
    {
        bool possible;
        size_t num_left;

        std::tie(possible, num_left) = check_if_vecs_possible(vec);

        return possible;
    }
    bool contains_all_digits(const std::vector<dtype> &vec)
    {
        bool possible;
        size_t num_left;

        std::tie(possible, num_left) = check_if_vecs_possible(vec);

        if ((possible == true) && (num_left == 0))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

public:
    Grid() : grid_size(0), block_len(0) {}
    Grid(size_t s) : grid_size(s), block_len((dtype)sqrt(s)), grid(s, GridRow(s)) {}
    Grid(size_t s, std::vector<std::vector<dtype>> g) : grid_size(s), block_len((dtype)sqrt(s))
    {
        assert(s == g.size());
        for (auto &vec : g)
        {
            assert(s == vec.size());
            grid.emplace_back(GridRow(vec));
        }
    }

    GridRow &operator[](size_t idx) { return grid[idx]; }

    // Below function refers to geeks for geek
    bool find_next_empty_cell(int &row, int &col)
    {
        for (row = 0; row < grid.size(); row++)
        {
            for (col = 0; col < grid.size(); col++)
            {
                if (!grid[row][col].is_solved())
                {
                    return true;
                }
            }
        }
        return false;
    }

    void display_values()
    {
        std::cout << "Displaying Sudoku Grid" << std::endl;
        std::cout << "Grid Size: " << grid_size << std::endl;
        std::cout << "Block Len: " << block_len << std::endl;

        // TODO: Make Display a little Nicer
        for (GridRow &g_row : grid)
        {
            for (auto &square : g_row.vec)
            {
                std::cout << square.value() << " ";
            }

            std::cout << std::endl;
        }
    }

    // Debug purpose
    std::string display_values_inline()
    {
        // std::cout << "Displaying Sudoku Grid" << std::endl;
        // std::cout << "Grid Size: " << grid_size << std::endl;
        // std::cout << "Block Len: " << block_len << std::endl;

        // TODO: Make Display a little Nicer
        std::string grid_string = "";
        for (GridRow &g_row : grid)
        {
            for (auto &square : g_row.vec)
            {
                grid_string += std::to_string(square.value()) + " ";
            }
        }
        // std::cout << std::endl;
        return grid_string;
    }
    std::vector<Coordinate> find_all_empty_cells()
    {
        std::vector<Coordinate> res;
        // for (size_t col = 0; col < grid_size; col++)
        // {
        //     for (size_t row = 0; row < grid_size; row++)
        //     {
                for (size_t row = 0; row < grid_size; row++)
        {
            for (size_t col= 0; col < grid_size; col++)
            {
                if (!grid[row][col].is_solved())
                {
                    res.emplace_back((Coordinate){.r = row, .c = col});
                }
            }
        }

        return res;
    }

    /*
    std::vector<Coordinate> find_all_empty_cells()
    {
        std::vector<Coordinate> res;
        for (size_t col = 0; col < grid_size; col++)
        {
            for (size_t row = 0; row < grid_size; row++)
            {
                if (!grid[row][col].is_solved())
                {
                    res.emplace_back((Coordinate){.r = row, .c = col});
                }
            }
        }

        return res;
    }
    */
    std::vector<Coordinate> find_all_empty_cells_parallel()
    {
        std::vector<Coordinate> res;
#pragma omp parallel
        {
            std::vector<Coordinate> private_res;
#pragma omp for nowait
            for (size_t col = 0; col < grid_size; col++)
            {
                for (size_t row = 0; row < grid_size; row++)
                {
                    if (!grid[row][col].is_solved())
                    {
                        private_res.emplace_back((Coordinate){.r = row, .c = col});
                    }
                }
            }
#pragma omp critical
            {
                res.insert(res.end(), private_res.begin(), private_res.end());
            }
        }
        return res;
    }

    /*
        void annotate_coords(std::vector<Coordinate> coords)
        {
            std::set<dtype, std::greater<dtype>> copy;

            for (dtype i = 1; i <= grid_size; i++)
            {
                copy.insert(i);
            }

            for (Coordinate coord : coords)
            {
                std::set<dtype, std::greater<dtype>> unseen = copy;

                size_t row = coord.r;
                size_t col = coord.c;

                for (size_t c = 0; c < grid_size; c++)
                {
                    if (grid[row][c].is_solved())
                    {
                        unseen.erase(grid[row][c].value());
                    }
                }

                for (size_t r = 0; r < grid_size; r++)
                {
                    if (grid[r][col].is_solved())
                    {
                        unseen.erase(grid[r][col].value());
                    }
                }

                size_t block_row_start = row - row % block_len;
                size_t block_col_start = col - col % block_len;
                for (size_t r = block_row_start; r < (block_row_start + block_len); r++)
                {
                    for (size_t c = block_col_start; c < (block_row_start + block_len); c++)
                    {
                        if (grid[r][c].is_solved())
                        {
                            unseen.erase(grid[r][c].value());
                        }
                    }
                }

                grid[row][col].set_choices(std::vector<dtype>(unseen.begin(), unseen.end()));
            }
        }
    */
    bool validate()
    {
        assert(block_len * block_len == grid_size);
        assert(grid.size() == grid_size);
        for (auto &gr : grid)
        {
            assert(gr.size() == grid_size);
        }

        if (empty())
        {
            return false;
        }

        // check all rows contains all digits
        for (auto &g_row : grid)
        {
            if (!contains_all_digits(g_row.values()))
            {
                return false;
            }
        }

        // checks if all cols contain all digits
        for (size_t col = 0; col < grid_size; col++)
        {
            std::vector<dtype> vec;
            for (size_t row = 0; row < grid_size; row++)
            {
                vec.emplace_back(grid[row][col].value());
            }

            if (!contains_all_digits(vec))
            {
                return false;
            }
        }

        // all blocks contain all the digits
        for (size_t row = 0; row < grid_size; row += block_len)
        {
            for (size_t col = 0; col < grid_size; col += block_len)
            {
                std::vector<dtype> vec;
                for (size_t r_i = row; r_i < (row + block_len); r_i++)
                {
                    for (size_t c_i = col; c_i < (col + block_len); c_i++)
                    {
                        vec.emplace_back(grid[r_i][c_i].value());
                    }
                }
                if (!contains_all_digits(vec))
                {
                    return false;
                }
            }
        }

        return true;
    }

    /* a copy of the validate but checks the entire grid
    this function is overkill and very slow... read BFS comments for TODO fix later */
    bool is_possible()
    {
        assert(block_len * block_len == grid_size);
        assert(grid.size() == grid_size);
        for (auto &gr : grid)
        {
            assert(gr.size() == grid_size);
        }

        if (empty())
        {
            return false;
        }

        // check all rows contains all digits
        for (auto &g_row : grid)
        {
            if (!is_possible(g_row.values()))
            {
                return false;
            }
        }

        // checks if all cols contain all digits
        for (size_t col = 0; col < grid_size; col++)
        {
            std::vector<dtype> vec;
            for (size_t row = 0; row < grid_size; row++)
            {
                vec.emplace_back(grid[row][col].value());
            }

            if (!is_possible(vec))
            {
                return false;
            }
        }

        // all blocks contain all the digits
        for (size_t row = 0; row < grid_size; row += block_len)
        {
            for (size_t col = 0; col < grid_size; col += block_len)
            {
                std::vector<dtype> vec;
                for (size_t r_i = row; r_i < (row + block_len); r_i++)
                {
                    for (size_t c_i = col; c_i < (col + block_len); c_i++)
                    {
                        vec.emplace_back(grid[r_i][c_i].value());
                    }
                }
                if (!is_possible(vec))
                {
                    return false;
                }
            }
        }

        return true;
    }
    size_t size()
    {
        return grid_size;
    }

    bool empty()
    {
        if (grid_size == 0)
        {
            return true;
        }
        return false;
    }

    // Build row array

    int get_box_index(size_t row, size_t col)
    {
        return row / block_len * block_len + col / block_len;
    }

    void set_square_choices(std::vector<Coordinate> empty_cells)
    {
        init_row_col_box();

        for (Coordinate cell : empty_cells)
        {
            int box_index = get_box_index(cell.r, cell.c);
            std::bitset<BIT_CNT> available_choice = ~(row[cell.r] | col[cell.c] | box[box_index]);
            grid[cell.r][cell.c].set_choices(available_choice);
            if (cell.r == 8 && cell.c == 7){
            std::cout << "set " << cell.r << ", " << cell.c << " to:" << grid[cell.r][cell.c].get_choices();
            }
        }
        print_arrays();

    }
    void get_row_array()
    {
        for (int row_index = 0; row_index < grid_size; row_index++)
        {
            for (int col_index = 0; col_index < grid_size; col_index++)
            {
                row[row_index] |= 1 << (grid[row_index][col_index].value() - 1);
            }
        }
    }

    void get_col_array()
    {
        for (int col_index = 0; col_index < grid_size; col_index++)
        {
            for (int row_index = 0; row_index < grid_size; row_index++)
            {
                col[col_index] |= 1 << (grid[row_index][col_index].value() - 1);
            }
        }
    }

    void get_box_array()
    {
        for (int box_index = 0; box_index < grid_size; box_index++)
        {
            int row_start = box_index / block_len * block_len;
            int col_start = box_index % block_len * block_len;
            for (int row_index = 0; row_index < block_len; row_index++)
            {
                for (int col_index = 0; col_index < block_len; col_index++)
                {
                    box[box_index] |= 1 << (grid[row_start + row_index][col_start + col_index].value() - 1);
                }
            }
        }
    }

    void init_row_col_box()
    {
        row.resize(grid_size);
        col.resize(grid_size);
        box.resize(grid_size);

        get_row_array();
        get_col_array();
        get_box_array();
    }
    void print_arrays()
    {
        display_values();

        std::cout << "choices: " << std::endl;

        for (int row_index = 0; row_index < grid_size; row_index++)
        {
            for (int col_index = 0; col_index < grid_size; col_index++)
            {
                std::cout << grid[row_index][col_index].get_choices() << " ";
            }
            std::cout << std::endl;
        }

        std::cout << "row: " << std::endl;

        for (int i = 0; i < grid_size; i++)
        {
            std::cout << row[i] << std::endl;
        }
        std::cout << "col: " << std::endl;

        for (int i = 0; i < grid_size; i++)
        {
            std::cout << col[i] << std::endl;
        }

        std::cout << "box: " << std::endl;

        for (int i = 0; i < grid_size; i++)
        {
            std::cout << box[i] << std::endl;
        }
    }
};

#endif
