#include <iostream>
#include <stdbool.h>
#include <vector>
#include <tuple>
#include <climits>

#include <cmath>
#include <cassert>

typedef int dtype; 

struct Coordinate{
  size_t r;
  size_t c;
};
typedef struct Coordinate Coordinate;

class Square {
private:
    dtype num; 
    //List of Possibilities

public:
    Square(): num(-1) {}
    Square(dtype val): num(val) {}

    bool is_solved() { return (num <= 0) ? false : true; };
    dtype& value() {
        return num;
    }
    
    //override = operator, so can assign value directly
    void operator= (const dtype num) {
        this->num = num;
    }
};

class Grid {
public:
    class GridRow
    {
        private:
            friend class Grid; // Allow Grid to Create & Access Internals
            std::vector<Square> vec;
            GridRow(const size_t size) : vec(size, Square()){}
            GridRow(const std::vector<dtype> v) {
                for(auto s: v) {
                    vec.emplace_back(Square(s));
                }
            } 
            
        public:
            Square& operator[](size_t idx) {return vec[idx];}
            size_t size() {return vec.size();}
            std::vector<dtype> values() {
                std::vector<dtype> res;
                for(auto sq: vec) {
                    res.emplace_back(sq.value());
                }
                return res;
            }
    };
    
private:
    size_t grid_size; //row & col 
    size_t block_len; // sqrt(row)

    std::vector<GridRow> grid;
    
    //checks is the vectors are possible in sudoku
    //returns : bool - true or false if possible 
    //   size_t - number of digits missing in row (only valid when bool is true)
    std::pair<bool, size_t> check_if_vecs_possible(const 
                                        std::vector<dtype>& vec) {
        if(vec.size() != grid_size) {
            return std::make_pair(false, 0 );
        }
        
        //use a vector to check if value has been seen
        std::vector<bool> seen(vec.size(), false);
        size_t num_left = grid_size;
        
        for(auto val: vec) {
            if(val <= 0) {
                //sudoku numbers are from 1 to gridsize
                continue;
            }
            
            auto val_index = val - 1;
            if(seen[val_index] == false) {
                seen[val_index] = true;
                num_left--;
            } else {
                //seen more than once 
                return std::make_pair(false, 0);
            }
        }
        
        return std::make_pair(true, num_left);
    }
    bool is_possible(const std::vector<dtype>& vec) {
        bool possible;
        size_t num_left;
        
        std::tie(possible, num_left) = check_if_vecs_possible(vec);
        
        return possible; 
    }
    bool contains_all_digits(const std::vector<dtype>& vec) {
        bool possible;
        size_t num_left;
        
        std::tie(possible, num_left) = check_if_vecs_possible(vec);
        
        if((possible == true) && (num_left == 0)) {
            return true;
        } else {
            return false;
        }
    }
public:
    Grid(): grid_size(0), block_len(0) {}
    Grid(size_t s) : grid_size(s), block_len((dtype)sqrt(s)), grid(s, GridRow(s)) {}
    Grid(size_t s, std::vector<std::vector<dtype>> g) : grid_size(s), block_len((dtype)sqrt(s)) {
        assert(s == g.size());
        for(auto& vec : g) {
            assert(s == vec.size());
            grid.emplace_back(GridRow(vec));
        }
        
    }
    GridRow& operator[](size_t idx) {return grid[idx];}
    
    void display_values() {
        std::cout << "Displaying Sudoku Grid" << std::endl;
        std::cout << "Grid Size: " << grid_size << std::endl;
        std::cout << "Block Len: " << block_len << std::endl;
        
        //TODO: Make Display a little Nicer 
        for(GridRow& g_row: grid) {
            for(auto& square: g_row.vec) {
                std::cout << square.value() << " ";
            }
            
            std::cout << std::endl;
        }
    }
    
    std::vector<Coordinate> find_all_empty_cells() {
        std::vector<Coordinate> res; 
        for(size_t col = 0; col < grid_size; col++) {
            for(size_t row = 0; row < grid_size; row ++) {
                if(!grid[row][col].is_solved()) {
                    res.emplace_back((Coordinate){.r = row, .c = col});
                }
            }
        }    
        
        return res;
    }
    
    bool validate() {
        assert(block_len*block_len == grid_size);
        assert(grid.size() == grid_size);
        for(auto& gr: grid) { assert(gr.size() == grid_size);}
        
        if(empty()) {
            return false;
        }
        
        //check all rows contains all digits 
        for(auto& g_row: grid) {
            if(!contains_all_digits(g_row.values())){
                return false;
            }
        }
        
        //checks if all cols contain all digits 
        for(size_t col = 0; col < grid_size; col++) {
            std::vector<dtype> vec;
            for(size_t row = 0; row < grid_size; row ++) {
                vec.emplace_back(grid[row][col].value());
            }
            
            if(!contains_all_digits(vec)){
                return false;
            }
        }
        
        //all blocks contain all the digits  
        for(size_t row = 0; row < grid_size; row +=block_len) {
            for(size_t col = 0; col < grid_size; col+=block_len) {
                std::vector<dtype> vec;
                for(size_t r_i = row; r_i < (row+block_len); r_i++) {
                    for(size_t c_i = col; c_i < (col+block_len); c_i++) {
                        vec.emplace_back(grid[r_i][c_i].value());
                    }
                }
                if(!contains_all_digits(vec)){
                    return false;
                }
            }
        }
        
        return true;
    }
    
    /* a copy of the validate but checks the entire grid 
    this function is overkill and very slow... read BFS comments for TODO fix later */
    bool is_possible() {
        assert(block_len*block_len == grid_size);
        assert(grid.size() == grid_size);
        for(auto& gr: grid) { assert(gr.size() == grid_size);}
        
        if(empty()) {
            return false;
        }
        
        //check all rows contains all digits 
        for(auto& g_row: grid) {
            if(!is_possible(g_row.values())){
                return false;
            }
        }
        
        //checks if all cols contain all digits 
        for(size_t col = 0; col < grid_size; col++) {
            std::vector<dtype> vec;
            for(size_t row = 0; row < grid_size; row ++) {
                vec.emplace_back(grid[row][col].value());
            }
            
            if(!is_possible(vec)){
                return false;
            }
        }
        
        //all blocks contain all the digits  
        for(size_t row = 0; row < grid_size; row +=block_len) {
            for(size_t col = 0; col < grid_size; col+=block_len) {
                std::vector<dtype> vec;
                for(size_t r_i = row; r_i < (row+block_len); r_i++) {
                    for(size_t c_i = col; c_i < (col+block_len); c_i++) {
                        vec.emplace_back(grid[r_i][c_i].value());
                    }
                }
                if(!is_possible(vec)){
                    return false;
                }
            }
        }
        
        return true;
    }
    size_t size() {
        return grid_size;
    }
    
    bool empty() {
        if(grid_size == 0) {
            return true;
        }
        return false;
    }
    

};