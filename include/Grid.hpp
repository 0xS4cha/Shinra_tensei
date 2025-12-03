#ifndef GRID_HPP
#define GRID_HPP

#include <vector>

#include <set>

struct Cell {
    int x, y;
    
    bool operator<(const Cell& other) const {
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};

class Grid {
private:
    std::set<Cell> aliveCells;
    
public:
    // Définir l'état d'une cellule
    void setCell(int x, int y, bool alive);
    
    // Vérifier si une cellule est vivante
    bool isAlive(int x, int y) const;
    
    // Compter les voisins vivants d'une cellule
    int countNeighbors(int x, int y) const;
    
    // Mettre à jour la grille selon les règles du Jeu de la Vie
    void update();
    
    // Obtenir l'ensemble des cellules vivantes
    const std::set<Cell>& getAliveCells() const;
};

#endif