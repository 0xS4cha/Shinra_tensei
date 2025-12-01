#include "Grid.hpp"

int Grid::getCell(int x, int y) {
    // À implémenter si nécessaire
    return 0;
}

void Grid::setCell(int x, int y, bool alive) {
    // À implémenter si nécessaire
}

void Grid::update() {
    // À implémenter si nécessaire
}

int Grid::countNeighbors(const std::vector<std::vector<int>>& grid, int x, int y, int rows, int cols) {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = (x + dx + cols) % cols;
            int ny = (y + dy + rows) % rows;
            count += grid[ny][nx];
        }
    }
    return count;
}

void Grid::iterate(std::vector<std::vector<int>>& grid, int rows, int cols) {
    std::vector<std::vector<int>> newGrid = grid;
    
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            int neighbors = countNeighbors(grid, x, y, rows, cols);
            
            if (grid[y][x] == 1) {
                // Cellule vivante
                newGrid[y][x] = (neighbors == 2 || neighbors == 3) ? 1 : 0;
            } else {
                // Cellule morte
                newGrid[y][x] = (neighbors == 3) ? 1 : 0;
            }
        }
    }
    
    grid = newGrid;
}

