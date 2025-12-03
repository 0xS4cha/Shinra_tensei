#include "Game.hpp"
#include <iostream>
#include <fstream>

Game::Game() : 
    WIDTH(1280), HEIGHT(720),
    window(nullptr), renderer(nullptr), font(nullptr),
    gameState(MAIN_MENU),
    camera_x(0.0f), camera_y(0.0f), zoom(1.0f),
    running(true), paused(true), last_update_time(0),
    history_index(-1), godModeActive(false),
    isPanning(false), panStartX(0), panStartY(0),
    isSelecting(false), selectionRect({0,0,0,0}) {

    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();
    
    window = SDL_CreateWindow("Game of Life - Shinra Tensei",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        // Handle error, maybe fallback to no text
    }

    // Main Menu Buttons
    newGameButton = { WIDTH / 2 - 100, HEIGHT / 2 - 80, 200, 50 };
    loadGameButton = { WIDTH / 2 - 100, HEIGHT / 2 - 20, 200, 50 };
    quitButton = { WIDTH / 2 - 100, HEIGHT / 2 + 40, 200, 50 };

    // In-Game UI Buttons
    playPauseButton = { WIDTH - UI_WIDTH + 20, 40, 210, 40 };
    nextStepButton = { WIDTH - UI_WIDTH + 20, 90, 210, 40 };
    undoButton = { WIDTH - UI_WIDTH + 20, 140, 100, 40 };
    redoButton = { WIDTH - UI_WIDTH + 130, 140, 100, 40 };
    clearButton = { WIDTH - UI_WIDTH + 20, 190, 210, 40 };
    randomizeButton = { WIDTH - UI_WIDTH + 20, 240, 210, 40 };
    godModeButton = { WIDTH - UI_WIDTH + 20, 290, 210, 40 };
    saveButton = { WIDTH - UI_WIDTH + 20, 340, 210, 40 };
    randomizeSelectionButton = { WIDTH - UI_WIDTH + 20, 390, 210, 40 };
    backToMenuButton = { WIDTH - UI_WIDTH + 20, HEIGHT - 60, 210, 40 };
}

Game::~Game() {
    if (font) TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
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
        }

        if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
            WIDTH = event.window.data1;
            HEIGHT = event.window.data2;
            // Update UI button positions based on new window size
            playPauseButton.x = WIDTH - UI_WIDTH + 20;
            nextStepButton.x = WIDTH - UI_WIDTH + 20;
            undoButton.x = WIDTH - UI_WIDTH + 20;
            redoButton.x = WIDTH - UI_WIDTH + 130;
            clearButton.x = WIDTH - UI_WIDTH + 20;
            randomizeButton.x = WIDTH - UI_WIDTH + 20;
            godModeButton.x = WIDTH - UI_WIDTH + 20;
            saveButton.x = WIDTH - UI_WIDTH + 20;
            randomizeSelectionButton.x = WIDTH - UI_WIDTH + 20;
            backToMenuButton.x = WIDTH - UI_WIDTH + 20;
            backToMenuButton.y = HEIGHT - 60;
        }

        if (gameState == IN_GAME) {
            handleGameEvents(event);
        } else { // MAIN_MENU
            handleMenuEvents(event);
        }
    }
}

void Game::handleGameEvents(SDL_Event& event) {
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                isPanning = true;
                panStartX = event.button.x - camera_x;
                panStartY = event.button.y - camera_y;
            } else if (event.button.button == SDL_BUTTON_LEFT) {
                if (event.button.x < WIDTH - UI_WIDTH) { // Click is on the grid
                    isSelecting = true;
                    selectionRect.x = event.button.x;
                    selectionRect.y = event.button.y;
                    selectionRect.w = 0;
                    selectionRect.h = 0;
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                isPanning = false;
            } else if (event.button.button == SDL_BUTTON_LEFT) {
                handleGameMouseClick(event.button); // Handle all left-click actions on mouse up
                isSelecting = false; // End selection process
            }
            break;

        case SDL_MOUSEMOTION:
            if (isPanning) {
                camera_x = event.motion.x - panStartX;
                camera_y = event.motion.y - panStartY;
            } else if (isSelecting) {
                selectionRect.w = event.motion.x - selectionRect.x;
                selectionRect.h = event.motion.y - selectionRect.y;
            }
            break;

        case SDL_MOUSEWHEEL:
            handleMouseWheel(event.wheel);
            break;
    }
}

void Game::handleMenuEvents(SDL_Event& event) {
    if (event.type == SDL_MOUSEBUTTONDOWN) {
        handleMenuMouseClick(event.button);
    }
}

void Game::handleMouseClick(SDL_MouseButtonEvent& b) {
    // This function is now a dispatcher
    if (gameState == IN_GAME) {
        handleGameMouseClick(b);
    } else {
        handleMenuMouseClick(b);
    }
}

void Game::handleMenuMouseClick(SDL_MouseButtonEvent& b) {
    if (b.button == SDL_BUTTON_LEFT) {
        if (b.x >= newGameButton.x && b.x <= newGameButton.x + newGameButton.w &&
            b.y >= newGameButton.y && b.y <= newGameButton.y + newGameButton.h) {
            gameState = IN_GAME;
            grid.clear();
            history.clear();
            history_index = -1;
            addToHistory();
        } else if (b.x >= loadGameButton.x && b.x <= loadGameButton.x + loadGameButton.w &&
                   b.y >= loadGameButton.y && b.y <= loadGameButton.y + loadGameButton.h) {
            if (grid.loadFromFile("save.dat")) {
                gameState = IN_GAME;
                history.clear();
                history_index = -1;
                addToHistory();
            }
        } else if (b.x >= quitButton.x && b.x <= quitButton.x + quitButton.w &&
                   b.y >= quitButton.y && b.y <= quitButton.y + quitButton.h) {
            running = false;
        }
    }
}

void Game::handleGameMouseClick(SDL_MouseButtonEvent& b) {
    // This function is now only called on MOUSEBUTTONUP for the left button.
    if (b.button != SDL_BUTTON_LEFT) return;

    // Check if it was a drag (selection) or a click
    bool wasDrag = (selectionRect.w != 0 || selectionRect.h != 0);

    if (b.x >= WIDTH - UI_WIDTH) {
        // UI click, ignore selection
        if (b.y >= playPauseButton.y && b.y <= playPauseButton.y + playPauseButton.h) {
            paused = !paused;
            if (!paused) last_update_time = SDL_GetTicks();
        } else if (b.y >= nextStepButton.y && b.y <= nextStepButton.y + nextStepButton.h) {
            if (paused) {
                grid.update(paused);
                addToHistory();
            }
        } else if (b.y >= undoButton.y && b.y <= undoButton.y + undoButton.h) {
            undo();
        } else if (b.y >= redoButton.y && b.y <= redoButton.y + redoButton.h) {
            redo();
        } else if (b.y >= clearButton.y && b.y <= clearButton.y + clearButton.h) {
            grid.clear();
            addToHistory();
        } else if (b.y >= randomizeButton.y && b.y <= randomizeButton.y + randomizeButton.h) {
            int grid_w = (WIDTH - UI_WIDTH) / (CELL_SIZE * zoom);
            int grid_h = HEIGHT / (CELL_SIZE * zoom);
            grid.randomize(grid_w, grid_h, -camera_x / (CELL_SIZE * zoom), -camera_y / (CELL_SIZE * zoom));
            addToHistory();
        } else if (b.y >= godModeButton.y && b.y <= godModeButton.y + godModeButton.h) {
            godModeActive = !godModeActive;
        } else if (b.y >= saveButton.y && b.y <= saveButton.y + saveButton.h) {
            grid.saveToFile("save.dat");
        } else if (b.y >= randomizeSelectionButton.y && b.y <= randomizeSelectionButton.y + randomizeSelectionButton.h) {
            if (wasDrag) {
                // Normalize the selection rectangle
                SDL_Rect normalizedRect = selectionRect;
                if (normalizedRect.w < 0) {
                    normalizedRect.x += normalizedRect.w;
                    normalizedRect.w = -normalizedRect.w;
                }
                if (normalizedRect.h < 0) {
                    normalizedRect.y += normalizedRect.h;
                    normalizedRect.h = -normalizedRect.h;
                }

                // Convert screen coordinates to grid coordinates
                int start_x = static_cast<int>((normalizedRect.x - camera_x) / (CELL_SIZE * zoom));
                int start_y = static_cast<int>((normalizedRect.y - camera_y) / (CELL_SIZE * zoom));
                int end_x = static_cast<int>((normalizedRect.x + normalizedRect.w - camera_x) / (CELL_SIZE * zoom));
                int end_y = static_cast<int>((normalizedRect.y + normalizedRect.h - camera_y) / (CELL_SIZE * zoom));
                
                grid.randomize_selection(start_x, start_y, end_x - start_x + 1, end_y - start_y + 1);
                addToHistory();
            }
        } else if (b.y >= backToMenuButton.y && b.y <= backToMenuButton.y + backToMenuButton.h) {
            gameState = MAIN_MENU;
        }
    } else if (!wasDrag) {
        // Grid click (not a drag)
        int grid_x = static_cast<int>((b.x - camera_x) / (CELL_SIZE * zoom));
        int grid_y = static_cast<int>((b.y - camera_y) / (CELL_SIZE * zoom));
        if (godModeActive) {
            grid.setGodCell(grid_x, grid_y, !grid.isGod(grid_x, grid_y));
        } else {
            grid.setCell(grid_x, grid_y, !grid.isAlive(grid_x, grid_y));
            addToHistory();
        }
    }
    // Reset selection rectangle after every left mouse up
    selectionRect = {0, 0, 0, 0};
}

void Game::handleMouseWheel(SDL_MouseWheelEvent& wheel) {
    float old_zoom = zoom;
    if (wheel.y > 0) {
        zoom *= 1.1f;
    } else if (wheel.y < 0) {
        zoom /= 1.1f;
    }
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    camera_x = mouse_x - (mouse_x - camera_x) * (zoom / old_zoom);
    camera_y = mouse_y - (mouse_y - camera_y) * (zoom / old_zoom);
}

void Game::update() {
    Uint32 current_time = SDL_GetTicks();
    if (!paused && current_time > last_update_time + 200) { // Vitesse de simulation
        grid.update(paused);
        addToHistory();
        last_update_time = current_time;
    }
}

void Game::render() {
    SDL_SetRenderDrawColor(renderer, 20, 20, 30, 255);
    SDL_RenderClear(renderer);

    if (gameState == IN_GAME) {
        renderGrid();
        renderUI();
    } else { // MAIN_MENU
        renderMainMenu();
    }

    SDL_RenderPresent(renderer);
}

void Game::renderGrid() {
    float scaled_cell_size = CELL_SIZE * zoom;

    // Define the visible grid area
    float view_x_start = -camera_x;
    float view_y_start = -camera_y;
    float view_x_end = view_x_start + (WIDTH - UI_WIDTH);
    float view_y_end = view_y_start + HEIGHT;

    // Find the grid indices to start and end drawing
    int grid_x_start = floor(view_x_start / scaled_cell_size);
    int grid_y_start = floor(view_y_start / scaled_cell_size);
    int grid_x_end = ceil(view_x_end / scaled_cell_size);
    int grid_y_end = ceil(view_y_end / scaled_cell_size);

    // Draw grid lines if they are not too dense
    if (scaled_cell_size > 4) {
        SDL_SetRenderDrawColor(renderer, 40, 40, 50, 255);
        for (int x = grid_x_start; x <= grid_x_end; ++x) {
            int screen_x = round(x * scaled_cell_size + camera_x);
            SDL_RenderDrawLine(renderer, screen_x, 0, screen_x, HEIGHT);
        }
        for (int y = grid_y_start; y <= grid_y_end; ++y) {
            int screen_y = round(y * scaled_cell_size + camera_y);
            SDL_RenderDrawLine(renderer, 0, screen_y, WIDTH - UI_WIDTH, screen_y);
        }
    }

    // Draw living cells
    SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
    for (const auto& cell : grid.getAliveCells()) {
        if (cell.x >= grid_x_start && cell.x <= grid_x_end && cell.y >= grid_y_start && cell.y <= grid_y_end) {
            SDL_Rect r = {
                (int)round(cell.x * scaled_cell_size + camera_x),
                (int)round(cell.y * scaled_cell_size + camera_y),
                (int)round(scaled_cell_size),
                (int)round(scaled_cell_size)
            };
            if (r.x < WIDTH - UI_WIDTH) SDL_RenderFillRect(renderer, &r);
        }
    }

    // Draw God Mode cells
    SDL_SetRenderDrawColor(renderer, 255, 215, 0, 150); // Gold, semi-transparent
    for (const auto& cell : grid.getGodCells()) {
        if (cell.x >= grid_x_start && cell.x <= grid_x_end && cell.y >= grid_y_start && cell.y <= grid_y_end) {
            SDL_Rect r = {
                (int)round(cell.x * scaled_cell_size + camera_x),
                (int)round(cell.y * scaled_cell_size + camera_y),
                (int)round(scaled_cell_size),
                (int)round(scaled_cell_size)
            };
            if (r.x < WIDTH - UI_WIDTH) SDL_RenderFillRect(renderer, &r);
        }
    }

    // Render selection rectangle
    if (isSelecting) {
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 100); // Yellow, semi-transparent
        SDL_RenderFillRect(renderer, &selectionRect);
        SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Yellow, opaque border
        SDL_RenderDrawRect(renderer, &selectionRect);
    }
}

void Game::renderUI() {
    // Fond de l'UI
    SDL_Rect ui_bg = { WIDTH - UI_WIDTH, 0, UI_WIDTH, HEIGHT };
    SDL_SetRenderDrawColor(renderer, 50, 50, 60, 255);
    SDL_RenderFillRect(renderer, &ui_bg);

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_Color godColor = { 0, 0, 0, 255 };

    // Buttons
    // Play/Pause
    if (paused) {
        SDL_SetRenderDrawColor(renderer, 80, 180, 80, 255);
        SDL_RenderFillRect(renderer, &playPauseButton);
        renderText("Play", playPauseButton.x, playPauseButton.y, playPauseButton.w, playPauseButton.h, textColor);
    } else {
        SDL_SetRenderDrawColor(renderer, 180, 80, 80, 255);
        SDL_RenderFillRect(renderer, &playPauseButton);
        renderText("Pause", playPauseButton.x, playPauseButton.y, playPauseButton.w, playPauseButton.h, textColor);
    }

    // Other buttons
    SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
    SDL_RenderFillRect(renderer, &nextStepButton);
    renderText("Next Step", nextStepButton.x, nextStepButton.y, nextStepButton.w, nextStepButton.h, textColor);
    SDL_RenderFillRect(renderer, &undoButton);
    renderText("Undo", undoButton.x, undoButton.y, undoButton.w, undoButton.h, textColor);
    SDL_RenderFillRect(renderer, &redoButton);
    renderText("Redo", redoButton.x, redoButton.y, redoButton.w, redoButton.h, textColor);
    SDL_RenderFillRect(renderer, &clearButton);
    renderText("Clear", clearButton.x, clearButton.y, clearButton.w, clearButton.h, textColor);
    SDL_RenderFillRect(renderer, &randomizeButton);
    renderText("Randomize", randomizeButton.x, randomizeButton.y, randomizeButton.w, randomizeButton.h, textColor);

    // God Mode
    if (godModeActive) {
        SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255); // Gold
        SDL_RenderFillRect(renderer, &godModeButton);
        renderText("God Mode ON", godModeButton.x, godModeButton.y, godModeButton.w, godModeButton.h, godColor);
    } else {
        SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
        SDL_RenderFillRect(renderer, &godModeButton);
        renderText("God Mode OFF", godModeButton.x, godModeButton.y, godModeButton.w, godModeButton.h, textColor);
    }

    // Save, Randomize Selection, and Back to Menu buttons
    SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
    SDL_RenderFillRect(renderer, &saveButton);
    renderText("Save", saveButton.x, saveButton.y, saveButton.w, saveButton.h, textColor);
    SDL_RenderFillRect(renderer, &randomizeSelectionButton);
    renderText("Randomize Selection", randomizeSelectionButton.x, randomizeSelectionButton.y, randomizeSelectionButton.w, randomizeSelectionButton.h, textColor);
    SDL_RenderFillRect(renderer, &backToMenuButton);
    renderText("Back to Menu", backToMenuButton.x, backToMenuButton.y, backToMenuButton.w, backToMenuButton.h, textColor);
}

void Game::renderMainMenu() {
    // Update button positions to always be centered
    newGameButton.x = WIDTH / 2 - 100;
    newGameButton.y = HEIGHT / 2 - 80;
    loadGameButton.x = WIDTH / 2 - 100;
    loadGameButton.y = HEIGHT / 2 - 20;
    quitButton.x = WIDTH / 2 - 100;
    quitButton.y = HEIGHT / 2 + 40;

    SDL_Color textColor = { 255, 255, 255, 255 };
    SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
    SDL_RenderFillRect(renderer, &newGameButton);
    renderText("New Game", newGameButton.x, newGameButton.y, newGameButton.w, newGameButton.h, textColor);
    SDL_RenderFillRect(renderer, &loadGameButton);
    renderText("Load Game", loadGameButton.x, loadGameButton.y, loadGameButton.w, loadGameButton.h, textColor);
    SDL_RenderFillRect(renderer, &quitButton);
    renderText("Quit", quitButton.x, quitButton.y, quitButton.w, quitButton.h, textColor);
}

void Game::renderText(const char* text, int x, int y, int w, int h, SDL_Color color) {
    if (!font) return;
    SDL_Surface* surface = TTF_RenderText_Solid(font, text, color);
    if (!surface) {
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        std::cerr << "Failed to create text texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    // Center the text inside the button rect {x, y, w, h}
    SDL_Rect dstRect = { x + (w - surface->w) / 2, y + (h - surface->h) / 2, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::addToHistory() {
    if (history_index < history.size() - 1) {
        history.erase(history.begin() + history_index + 1, history.end());
    }
    history.push_back(grid.getAliveCells());
    history_index++;
}

void Game::undo() {
    if (history_index > 0) {
        history_index--;
        grid.setAliveCells(history[history_index]);
    }
}

void Game::redo() {
    if (history_index < history.size() - 1) {
        history_index++;
        grid.setAliveCells(history[history_index]);
    }
}