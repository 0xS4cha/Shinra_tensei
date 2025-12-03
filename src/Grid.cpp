#include <map>
#include "Grid.hpp"

#include <set>

void Grid::setCell(int x, int y, bool alive) {
    if (alive) {
        aliveCells.insert({x, y});
    } else {
        aliveCells.erase({x, y});
    }
}

bool Grid::isAlive(int x, int y) const {
    return aliveCells.find({x, y}) != aliveCells.end();
}

int Grid::countNeighbors(int x, int y) const {
    int count = 0;
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) continue;
            if (isAlive(x + dx, y + dy)) count++;
        }
    }
    return count;
}

void Grid::update() {
    std::set<Cell> cellsToCheck;

    // Vérifier toutes les cellules vivantes et leurs voisins
    for (const auto& cell : aliveCells) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                cellsToCheck.insert({cell.x + dx, cell.y + dy});
            }
        }
    }

    std::set<Cell> newAliveCells;

    for (const auto& cell : cellsToCheck) {
        int neighbors = countNeighbors(cell.x, cell.y);
        bool currentlyAlive = isAlive(cell.x, cell.y);

        if (currentlyAlive && (neighbors == 2 || neighbors == 3)) {
            newAliveCells.insert(cell);
        } else if (!currentlyAlive && neighbors == 3) {
            newAliveCells.insert(cell);
        }
    }

    aliveCells = newAliveCells;
}

const std::set<Cell>& Grid::getAliveCells() const {
    return aliveCells;
}
