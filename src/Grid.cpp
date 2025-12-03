#include <map>
#include "Grid.hpp"

#include <set>
#include <cstdlib> // Pour rand()
#include <ctime>   // Pour time()
#include <fstream> // Pour std::ofstream et std::ifstream

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

void Grid::clear() {
    aliveCells.clear();
}

void Grid::randomize(int width, int height, int x_offset, int y_offset) {
    clear();
    srand(time(NULL));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (rand() % 5 == 0) { // 1 chance sur 5 d'être vivante
                aliveCells.insert({x + x_offset, y + y_offset});
            }
        }
    }
}

void Grid::randomize_selection(int start_x, int start_y, int width, int height) {
    srand(time(NULL));
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (rand() % 2 == 0) { // 50% chance
                setCell(start_x + x, start_y + y, true);
            }
        }
    }
}

void Grid::setGodCell(int x, int y, bool isGod) {
    if (isGod) {
        godCells.insert({x, y});
    } else {
        godCells.erase({x, y});
    }
}

bool Grid::isGod(int x, int y) const {
    return godCells.count({x, y}) > 0;
}

const std::set<Cell>& Grid::getGodCells() const {
    return godCells;
}

void Grid::setAliveCells(const std::set<Cell>& cells) {
    aliveCells = cells;
}

void Grid::update(bool& simPaused) {
    if (simPaused) return;

    std::set<Cell> cellsToCheck;
    for (const auto& cell : aliveCells) {
        for (int dx = -1; dx <= 1; dx++) {
            for (int dy = -1; dy <= 1; dy++) {
                cellsToCheck.insert({cell.x + dx, cell.y + dy});
            }
        }
    }

    std::set<Cell> newAliveCells;
    for (const auto& cell : cellsToCheck) {
        if (isGod(cell.x, cell.y)) {
            if (isAlive(cell.x, cell.y)) {
                newAliveCells.insert(cell);
            }
            continue;
        }

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

bool Grid::saveToFile(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs) return false;

    size_t alive_count = aliveCells.size();
    ofs.write(reinterpret_cast<const char*>(&alive_count), sizeof(alive_count));
    for (const auto& cell : aliveCells) {
        ofs.write(reinterpret_cast<const char*>(&cell), sizeof(cell));
    }

    size_t god_count = godCells.size();
    ofs.write(reinterpret_cast<const char*>(&god_count), sizeof(god_count));
    for (const auto& cell : godCells) {
        ofs.write(reinterpret_cast<const char*>(&cell), sizeof(cell));
    }

    return true;
}

bool Grid::loadFromFile(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs) return false;

    aliveCells.clear();
    godCells.clear();

    size_t alive_count;
    ifs.read(reinterpret_cast<char*>(&alive_count), sizeof(alive_count));
    for (size_t i = 0; i < alive_count; ++i) {
        Cell cell;
        ifs.read(reinterpret_cast<char*>(&cell), sizeof(cell));
        aliveCells.insert(cell);
    }

    size_t god_count;
    ifs.read(reinterpret_cast<char*>(&god_count), sizeof(god_count));
    for (size_t i = 0; i < god_count; ++i) {
        Cell cell;
        ifs.read(reinterpret_cast<char*>(&cell), sizeof(cell));
        godCells.insert(cell);
    }

    return true;
}
