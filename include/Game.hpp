#ifndef GAME_HPP
#define GAME_HPP

#include <SDL2/SDL.h>
#include <vector>

class Game {
private:
    static const int CELL_SIZE = 20;
    int WIDTH;
    int HEIGHT;
    int COLS;
    int ROWS;
    
    SDL_Window* mainWindow;
    SDL_Renderer* mainRenderer;
    SDL_Window* controlWindow;
    SDL_Renderer* controlRenderer;
    
    std::vector<std::vector<int>> grid;
    
    int x_pos;
    int y_pos;
    
    SDL_Rect skipButton;
    SDL_Rect preSkipButton;
    
    bool running;
    
    void handleEvents();
    void update();
    void render();
    void handleMainWindowClick(int x, int y);
    void handleControlWindowClick(int x, int y);
    void renderMainWindow();
    void renderControlWindow();

public:
    Game();
    ~Game();
    
    void run();
};

#endif