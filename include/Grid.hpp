#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

class Grid {
public:
    static int getCell(int x, int y);
    static void setCell(int x, int y, bool alive);
    static void update();
    static void iterate(std::vector<std::vector<int>>& grid, int rows, int cols);
    static int countNeighbors(const std::vector<std::vector<int>>& grid, int x, int y, int rows, int cols);
};

#endif