#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <vector>
#include "Grid.hpp"

class Game {
private:
    enum GameState {
        MAIN_MENU,
        IN_GAME
    };

    static const int CELL_SIZE = 20;
    const int UI_WIDTH = 250; // Largeur du panneau d'interface
    int WIDTH;
    int HEIGHT;
    
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font; // Font for rendering text
    
    Grid grid;
    GameState gameState;
    
    // Caméra et zoom
    float camera_x;
    float camera_y;
    float zoom;

    // État de la simulation
    bool running;
    bool paused;
    Uint32 simulation_speed_ms; // Vitesse de simulation en ms
    int generation_count;
    Uint32 last_update_time;

    // Historique
    std::vector<std::set<Cell>> history;
    int history_index;

    // In-Game UI Buttons
    SDL_Rect playPauseButton;
    SDL_Rect nextStepButton;
    SDL_Rect undoButton;
    SDL_Rect redoButton;
    SDL_Rect clearButton;
    SDL_Rect randomizeButton;
    SDL_Rect godModeButton;
    SDL_Rect saveButton;
    SDL_Rect backToMenuButton;
    SDL_Rect speedUpButton;
    SDL_Rect slowDownButton;
    SDL_Rect changeRulesButton;

    // Main Menu UI Buttons
    SDL_Rect newGameButton;
    SDL_Rect loadGameButton;
    SDL_Rect quitButton;

    bool godModeActive; // Pour savoir si on place des cellules en mode Dieu

    // Mouse state for panning and selection
    bool isPanning;
    int panStartX, panStartY;
    bool isSelecting;
    bool isDrawing; // Pour le nouveau mode dessin
    SDL_Rect selectionRect;

    // New button for randomizing selection
    SDL_Rect randomizeSelectionButton;

    void handleEvents();
    void handleGameEvents(SDL_Event& event);
    void handleMenuEvents(SDL_Event& event);

    void update();
    void render();

    // Nouvelles fonctions de gestion
    void handleMouseClick(SDL_MouseButtonEvent& b);
    void handleMenuMouseClick(SDL_MouseButtonEvent& b);
    void handleGameMouseClick(SDL_MouseButtonEvent& b);
    void handleMouseWheel(SDL_MouseWheelEvent& wheel);
    void renderUI();
    void renderGrid();
    void renderMainMenu();
    void renderText(const char* text, int x, int y, int w, int h, SDL_Color color);

    void addToHistory();
    void undo();
    void redo();

public:
    Game();
    ~Game();
    
    void run();
};

#endif