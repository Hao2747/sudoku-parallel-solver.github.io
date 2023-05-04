#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>
#include <climits>

#include <cmath>
#include <cassert>
#include <omp.h>
#include <set>
#include <algorithm>

#ifndef GRID_H
#define GRID_H

typedef int dtype;

static int UNASSIGNED = 0;

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
    //std::bitset<BIT_CNT> choices;
    std::set<dtype> choices_;

public:
    Square() : num(UNASSIGNED) {
          // for (dtype i = 1; i <= grid_size; i++)
          // {
              // choices_.insert(i);
          // }
    }
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

    std::set<dtype> get_choices()
    {
        return choices_;
    }

    size_t get_choices_cnt(){
        return choices_.size();
    }

    void set_choices(std::set<dtype> &c) {
      choices_ = c;
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
    std::vector<std::set<dtype>> row_;
    std::vector<std::set<dtype>> col_;
    std::vector<std::set<dtype>> box_;
    std::vector<std::vector<Square *>> square_lookup;
    std::vector<Coordinate> private_coords;


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

    
      // void annotate_coords(std::vector<Coordinate> coords)
      // {
          // std::set<dtype, std::greater<dtype>> copy;

          // for (dtype i = 1; i <= grid_size; i++)
          // {
              // copy.insert(i);
          // }

          // for (Coordinate coord : coords)
          // {
              // std::set<dtype, std::greater<dtype>> unseen = copy;

              // size_t row = coord.r;
              // size_t col = coord.c;

              // for (size_t c = 0; c < grid_size; c++)
              // {
                  // if (grid[row][c].is_solved())
                  // {
                      // unseen.erase(grid[row][c].value());
                  // }
              // }

              // for (size_t r = 0; r < grid_size; r++)
              // {
                  // if (grid[r][col].is_solved())
                  // {
                      // unseen.erase(grid[r][col].value());
                  // }
              // }

              // size_t block_row_start = row - row % block_len;
              // size_t block_col_start = col - col % block_len;
              // for (size_t r = block_row_start; r < (block_row_start + block_len); r++)
              // {
                  // for (size_t c = block_col_start; c < (block_row_start + block_len); c++)
                  // {
                      // if (grid[r][c].is_solved())
                      // {
                          // unseen.erase(grid[r][c].value());
                      // }
                  // }
              // }

              // grid[row][col].set_choices(std::vector<dtype>(unseen.begin(), unseen.end()));
          // }
      // }
    bool validate()
    {
        assert(block_len * block_len == grid_size);
        if(grid.size() !=grid_size) {
          std::cout << grid.size() << std::endl;
          std::cout << grid.size() << std::endl;
        }
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
    
    
    
    bool is_possible(int row, int col)
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

        // check row contains all digits

        if (!is_possible(grid[row].values()))
        {
            return false;
        }


        // checks if col contain all digits

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

        // block contains all the digits
        for (size_t row_i= row/block_len * block_len; row_i < (row/block_len * block_len) + block_len; row_i ++)
        {
            for (size_t col_i =  col/block_len * block_len; col_i < col/block_len * block_len + block_len; col_i ++)
            {
                std::vector<dtype> vec;

                vec.emplace_back(grid[row_i][col_i].value());
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

    //used in cp backsolve1
    void set_square_choices(std::vector<Coordinate> empty_cells)
    {
      init_row_col_box();


      for (size_t row = 0; row < grid_size; row++)
      {
          for (size_t col= 0; col < grid_size; col++)
          {
              if (grid[row][col].is_solved())
              {
                dtype seen_value = grid[row][col].value();
                row_[row].erase(seen_value);
                col_[col].erase(seen_value);
                box_[get_box_index(row, col)].erase(seen_value);
              }
          }
      }

      for (Coordinate cell : empty_cells) {
        int box_index = get_box_index(cell.r, cell.c);
        
        std::set<dtype> available_choice= box_[box_index];
        
        for(dtype d : row_[cell.r]) {
          if(!available_choice.count(d)) {
            available_choice.erase(d);
          }
        }
        for(dtype d : col_[cell.c]) {
          if(!available_choice.count(d)) {
            available_choice.erase(d);
          }
        }
        // if(available_choice.size() == 1) {
          // //optimization for one element left
          // // for(dtype d : available_choice) {
              // // grid[cell.r][cell.c] = d;
          // // }
        // } else {
          grid[cell.r][cell.c].set_choices(available_choice);
        //}
      }

      //display_values();
      //print_arrays();
    }

    std::vector<Coordinate> get_coords() {
      return private_coords;
    }
    
    void seq_init_choices()
    {
      init_row_col_box();

      for (size_t row = 0; row < grid_size; row++)
      {
          for (size_t col= 0; col < grid_size; col++)
          {
              if (grid[row][col].is_solved())
              {
                dtype seen_value = grid[row][col].value();
                row_[row].erase(seen_value);
                col_[col].erase(seen_value);
                box_[get_box_index(row, col)].erase(seen_value);
                square_lookup[row].emplace_back(nullptr);
              } else {
                square_lookup[row].emplace_back(&grid[row][col]);
                private_coords.emplace_back((Coordinate){.r = row, .c = col});
              }
          }
      }
    }
    
    Coordinate pop_coord() {
      
      Coordinate curr = private_coords.back();
      private_coords.pop_back();
      int row = curr.r;
      int col = curr.c; 
      
      square_lookup[row][col] = nullptr;
      return curr;
      
    }
    
    std::set<dtype> find_choices_from_coord(size_t row, size_t col) {
      int box_index = get_box_index(row, col);
      
      std::set<dtype> available_choice;

      std::vector<unsigned short> counts(grid_size+1, 0);
      
      for(dtype d : row_[row]) {
        counts[d]++;
      }
      for(dtype d : col_[col]) {
        counts[d]++;
      }
      
      for(dtype d : box_[box_index]) {
        counts[d]++;
      }
      
      for(int i = 1; i < counts.size();i++) {
        if(counts[i] == 3) {
          available_choice.insert(i);
        }
      }
      
      return available_choice;
      
    }

    // void par_set_square_choices(std::vector<Coordinate> empty_cells)
    // {
      // init_row_col_box();


      // for (size_t row = 0; row < grid_size; row++)
      // {
          // for (size_t col= 0; col < grid_size; col++)
          // {
              // if (grid[row][col].is_solved())
              // {
                // dtype seen_value = grid[row][col].value();
                // row_[row].erase(seen_value);
                // col_[col].erase(seen_value);
                // box_[get_box_index(row, col)].erase(seen_value);
                // square_lookup[row].emplace_back(&grid[row][col]);
              // } else {
                // square_lookup[row].emplace_back(nullptr);
              // }
          // }
      // }

      // #pragma omp parallel for schedule(dynamic)
      // for (Coordinate cell : empty_cells) {
        // int box_index = get_box_index(cell.r, cell.c);
        
        // std::set<dtype> available_choice= box_[box_index];
        
        // for(dtype d : row_[cell.r]) {
          // if(!available_choice.count(d)) {
            // available_choice.erase(d);
          // }
        // }
        // for(dtype d : col_[cell.c]) {
          // if(!available_choice.count(d)) {
            // available_choice.erase(d);
          // }
        // }
        // // if(available_choice.size() == 1) {
          // // //optimization for one element left
          // // // for(dtype d : available_choice) {
              // // // grid[cell.r][cell.c] = d;
          // // // }
        // // } else {
          // grid[cell.r][cell.c].set_choices(available_choice);
        // //}
      // }

      // //display_values();
      // //print_arrays();
    // }
    
    void update_annotations(size_t row, size_t col, dtype guess) {
      row_[row].erase(guess);
      col_[col].erase(guess);
      box_[get_box_index(row,col)].erase(guess);
    }
    
    void init_row_col_box()
    {
        row_.resize(grid_size);
        col_.resize(grid_size);
        box_.resize(grid_size);

        std::set<dtype> copy;
        for (dtype i = 1; i <= grid_size; i++)
        {
            copy.insert(i);
        }
        for(int i = 0; i < grid_size; i++) {
          row_[i]=(copy);
          col_[i]=(copy);
          box_[i]=(copy);
        }

        square_lookup.resize(grid_size);
        
    }
    
    //todo make private 
    void print_set(std::vector<std::set<dtype>> &annotate_vec, std::string annotate_descriptor) {
      std::cout << annotate_descriptor << std::endl;
      
      for (int i = 0; i < grid_size; i++) {

        if(annotate_vec[i].size() == 0) {
          std::cout << "Has all elements";
          
        } else {
          for(dtype d : annotate_vec[i]) {
            std::cout << d << " ";
          }
        }
        std::cout << std::endl;
      }
      
      std::cout << std::endl;
    }
    void print_arrays() {
      std::cout << "choices: " << std::endl;

      for (int row_index = 0; row_index < grid_size; row_index++)
      {
          for (int col_index = 0; col_index < grid_size; col_index++)
          {
            for(dtype d : grid[row_index][col_index].get_choices()) {
              std::cout << d << " ";
            }
          }
          std::cout << std::endl;
      }

      print_set(row_, "row");
      print_set(col_, "col");
      print_set(box_, "box");
    }
    
};


#endif
