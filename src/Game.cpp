#include "Game.hpp"
#include "Grid.hpp"



Game::Game() : WIDTH(0), HEIGHT(0), COLS(0), ROWS(0), 
               mainWindow(nullptr), mainRenderer(nullptr),
               controlWindow(nullptr), controlRenderer(nullptr),
               x_pos(0), y_pos(0), running(true) {
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    WIDTH = dm.w;
    HEIGHT = dm.h;
    COLS = WIDTH / CELL_SIZE;
    ROWS = HEIGHT / CELL_SIZE;
    
    mainWindow = SDL_CreateWindow("Game of Life",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);
    
    controlWindow = SDL_CreateWindow("Controls",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        200, 100, SDL_WINDOW_SHOWN);
    controlRenderer = SDL_CreateRenderer(controlWindow, -1, SDL_RENDERER_ACCELERATED);

    // NOTE: don't create a local Grid named `grid` here — that would shadow
    // the member `grid` declared in Game.hpp. The member will be default-constructed.
    skipButton = { 20, 20, 70, 40 };
    preSkipButton = { 110, 20, 70, 40 };
}

Game::~Game() {
    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_DestroyRenderer(controlRenderer);
    SDL_DestroyWindow(controlWindow);
    SDL_Quit();
}

void Game::run() {
    while (running) {
        handleEvents();
        update();
        render();
        SDL_Delay(16);
    }
}

void Game::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
                case SDLK_LEFT:
                    x_pos -= CELL_SIZE;
                    break;
                case SDLK_RIGHT:
                    x_pos += CELL_SIZE;
                    break;
                case SDLK_UP:
                    y_pos -= CELL_SIZE;
                    break;
                case SDLK_DOWN:
                    y_pos += CELL_SIZE;
                    break;
            }
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (event.window.windowID == SDL_GetWindowID(mainWindow)) {
                int x = (event.button.x - x_pos) / CELL_SIZE;
                int y = (event.button.y - y_pos) / CELL_SIZE;
                handleMainWindowClick(x, y);
            } else if (event.window.windowID == SDL_GetWindowID(controlWindow)) {
                handleControlWindowClick(event.button.x, event.button.y);
            }
        }
    }
}

void Game::handleMainWindowClick(int x, int y) {
    grid.setCell(x, y, !(grid.isAlive(x, y)));
}

void Game::handleControlWindowClick(int mx, int my) {
    if (mx >= skipButton.x && mx <= skipButton.x + skipButton.w &&
        my >= skipButton.y && my <= skipButton.y + skipButton.h) {
        grid.update();
    } else if (mx >= preSkipButton.x && mx <= preSkipButton.x + preSkipButton.w &&
               my >= preSkipButton.y && my <= preSkipButton.y + preSkipButton.h) {
        // PreSkip : à implémenter si tu veux un historique
    }
}

void Game::render() {
    renderMainWindow();
    renderControlWindow();
}

void Game::update() {
    
}

void Game::renderMainWindow() {
    // --- Background ---
    SDL_SetRenderDrawColor(mainRenderer, 15, 15, 20, 255);  // fond gris-bleu sombre
    SDL_RenderClear(mainRenderer);
    
    // --- Grille ---
    SDL_SetRenderDrawColor(mainRenderer, 80, 90, 110, 255); // gris bleuté
    for (int y = 0; y <= ROWS; ++y)
        SDL_RenderDrawLine(mainRenderer, 0, y * CELL_SIZE + y_pos, WIDTH, y * CELL_SIZE + y_pos);

    for (int x = 0; x <= COLS; ++x)
        SDL_RenderDrawLine(mainRenderer, x * CELL_SIZE + x_pos, 0, x * CELL_SIZE + x_pos, HEIGHT);
    
    // --- Cellules vivantes ---
    SDL_SetRenderDrawColor(mainRenderer, 83, 219, 142, 255); // bleu clair vif

    for (Cell elemcell : grid.getAliveCells()) {
        SDL_Rect cell = { elemcell.x * CELL_SIZE + x_pos, elemcell.y * CELL_SIZE + y_pos, CELL_SIZE, CELL_SIZE };
        SDL_RenderFillRect(mainRenderer, &cell);
    }
    
    SDL_RenderPresent(mainRenderer);
}

void Game::renderControlWindow() {
    SDL_SetRenderDrawColor(controlRenderer, 50, 50, 50, 255);
    SDL_RenderClear(controlRenderer);
    
    SDL_SetRenderDrawColor(controlRenderer, 0, 0, 255, 255);
    SDL_RenderFillRect(controlRenderer, &skipButton);
    
    SDL_SetRenderDrawColor(controlRenderer, 255, 0, 0, 255);
    SDL_RenderFillRect(controlRenderer, &preSkipButton);
    
    SDL_RenderPresent(controlRenderer);
}