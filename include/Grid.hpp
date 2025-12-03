#ifndef GRID_HPP
#define GRID_HPP

#include <set>
#include <string>
#include <vector>

struct Cell {
    int x, y;
    
    bool operator<(const Cell& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }

    bool operator==(const Cell& other) const {
        return x == other.x && y == other.y;
    }
};

class Grid {
private:
    std::set<Cell> aliveCells;
    std::set<Cell> godCells; // Cellules en mode Dieu

public:
    // Définir l'état d'une cellule
    void setCell(int x, int y, bool alive);
    
    // Vérifier si une cellule est vivante
    bool isAlive(int x, int y) const;
    
    // Compter les voisins vivants d'une cellule
    int countNeighbors(int x, int y) const;
    
    // Mettre à jour la grille selon les règles du Jeu de la Vie
    void update(bool& simPaused);
    
    // Obtenir l'ensemble des cellules vivantes
    const std::set<Cell>& getAliveCells() const;

    // Nouvelles fonctions
    void clear();
    void randomize(int width, int height, int x_offset, int y_offset);
    void randomize_selection(int start_x, int start_y, int width, int height);
    void setGodCell(int x, int y, bool isGod);
    bool isGod(int x, int y) const;
    const std::set<Cell>& getGodCells() const;
    void setAliveCells(const std::set<Cell>& cells);

    // Save/Load
    bool saveToFile(const std::string& filename);
    bool loadFromFile(const std::string& filename);
};

#endif