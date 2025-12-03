#include "Game.hpp"
#include <iostream>
#include <fstream>

Game::Game() : 
    WIDTH(1280), HEIGHT(720),
    window(nullptr), renderer(nullptr), font(nullptr),
    gameState(MAIN_MENU),
    camera_x(0.0f), camera_y(0.0f), zoom(1.0f), 
    running(true), paused(true), simulation_speed_ms(200), generation_count(0), last_update_time(0),
    history_index(-1), godModeActive(false),
    isPanning(false), panStartX(0), panStartY(0),
    isSelecting(false), isDrawing(false), selectionRect({0,0,0,0}) {

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
    slowDownButton = { WIDTH - UI_WIDTH + 20, 440, 100, 40 };
    speedUpButton = { WIDTH - UI_WIDTH + 130, 440, 100, 40 };
    changeRulesButton = { WIDTH - UI_WIDTH + 20, 540, 210, 40 };
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
            slowDownButton.x = WIDTH - UI_WIDTH + 20;
            speedUpButton.x = WIDTH - UI_WIDTH + 130;
            changeRulesButton.x = WIDTH - UI_WIDTH + 20;
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
                const Uint8* keyboardState = SDL_GetKeyboardState(NULL);
                if (keyboardState[SDL_SCANCODE_LSHIFT] || keyboardState[SDL_SCANCODE_RSHIFT]) {
                    isDrawing = true;
                    // Activer la cellule sous le curseur immédiatement
                    handleGameMouseClick(event.button);
                } else {
                    if (event.button.x < WIDTH - UI_WIDTH) { // Click is on the grid
                        isSelecting = true;
                        selectionRect.x = event.button.x;
                        selectionRect.y = event.button.y;
                        selectionRect.w = 0;
                        selectionRect.h = 0;
                    }
                }
            }
            break;

        case SDL_MOUSEBUTTONUP:
            if (event.button.button == SDL_BUTTON_RIGHT) {
                isPanning = false;
            } else if (event.button.button == SDL_BUTTON_LEFT) {
                handleGameMouseClick(event.button); // Handle all left-click actions on mouse up
                isDrawing = false;
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
            } else if (isDrawing) {
                int grid_x = floor((event.motion.x - camera_x) / (CELL_SIZE * zoom));
                int grid_y = floor((event.motion.y - camera_y) / (CELL_SIZE * zoom));
                grid.setCell(grid_x, grid_y, true);
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
            generation_count = 0;
            addToHistory();
        } else if (b.x >= loadGameButton.x && b.x <= loadGameButton.x + loadGameButton.w &&
                   b.y >= loadGameButton.y && b.y <= loadGameButton.y + loadGameButton.h) {
            if (grid.loadFromFile("save.dat")) {
                gameState = IN_GAME;
                history.clear();
                history_index = -1;
                generation_count = 0; // Ou charger depuis le fichier de sauvegarde si vous l'ajoutez
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
    bool wasSelection = (isSelecting && (selectionRect.w != 0 || selectionRect.h != 0));

    if (b.x >= WIDTH - UI_WIDTH) {
        // UI click, ignore selection
        if (b.y >= playPauseButton.y && b.y <= playPauseButton.y + playPauseButton.h) {
            paused = !paused;
            if (!paused) last_update_time = SDL_GetTicks();
        } else if (b.y >= nextStepButton.y && b.y <= nextStepButton.y + nextStepButton.h) {
            if (paused) {
                grid.update(paused);
                generation_count++;
                addToHistory();
            }
        } else if (b.y >= undoButton.y && b.y <= undoButton.y + undoButton.h) {
            if (b.x >= undoButton.x && b.x <= undoButton.x + undoButton.w) {
            undo();
            // Note: generation_count n'est pas restauré, il reflète les étapes de simulation.
            } else if (b.x >= redoButton.x && b.x <= redoButton.x + redoButton.w) {
            redo();
            }
        } else if (b.x >= clearButton.x && b.x <= clearButton.x + clearButton.w &&
                   b.y >= clearButton.y && b.y <= clearButton.y + clearButton.h) {
            grid.clear();
            generation_count = 0;
            addToHistory();
        } else if (b.x >= randomizeButton.x && b.x <= randomizeButton.x + randomizeButton.w &&
                   b.y >= randomizeButton.y && b.y <= randomizeButton.y + randomizeButton.h) {
            int grid_w = (WIDTH - UI_WIDTH) / (CELL_SIZE * zoom);
            int grid_h = HEIGHT / (CELL_SIZE * zoom);
            generation_count = 0;
            grid.randomize(grid_w, grid_h, -camera_x / (CELL_SIZE * zoom), -camera_y / (CELL_SIZE * zoom));
            addToHistory();
        } else if (b.x >= godModeButton.x && b.x <= godModeButton.x + godModeButton.w &&
                   b.y >= godModeButton.y && b.y <= godModeButton.y + godModeButton.h) {
            godModeActive = !godModeActive;
        } else if (b.x >= saveButton.x && b.x <= saveButton.x + saveButton.w &&
                   b.y >= saveButton.y && b.y <= saveButton.y + saveButton.h) {
            grid.saveToFile("save.dat");
        } else if (b.x >= randomizeSelectionButton.x && b.x <= randomizeSelectionButton.x + randomizeSelectionButton.w &&
                   b.y >= randomizeSelectionButton.y && b.y <= randomizeSelectionButton.y + randomizeSelectionButton.h) {
            if (wasSelection) {
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
                int start_x = floor((normalizedRect.x - camera_x) / (CELL_SIZE * zoom));
                int start_y = floor((normalizedRect.y - camera_y) / (CELL_SIZE * zoom));
                int end_x = floor((normalizedRect.x + normalizedRect.w - camera_x) / (CELL_SIZE * zoom));
                int end_y = floor((normalizedRect.y + normalizedRect.h - camera_y) / (CELL_SIZE * zoom));
                
                grid.randomize_selection(start_x, start_y, end_x - start_x + 1, end_y - start_y + 1);
                addToHistory();
            }
        } else if (b.x >= backToMenuButton.x && b.x <= backToMenuButton.x + backToMenuButton.w &&
                   b.y >= backToMenuButton.y && b.y <= backToMenuButton.y + backToMenuButton.h) {
            gameState = MAIN_MENU;
        } else if (b.y >= slowDownButton.y && b.y <= slowDownButton.y + slowDownButton.h) {
            if (b.x >= slowDownButton.x && b.x <= slowDownButton.x + slowDownButton.w) {
                simulation_speed_ms += 50;
            } else if (b.x >= speedUpButton.x && b.x <= speedUpButton.x + speedUpButton.w) {
                if (simulation_speed_ms >= 50) simulation_speed_ms -= 50;
            }
        } else if (b.x >= changeRulesButton.x && b.x <= changeRulesButton.x + changeRulesButton.w &&
                   b.y >= changeRulesButton.y && b.y <= changeRulesButton.y + changeRulesButton.h) {
            RuleSet current_rules = grid.getRuleSet(); // Assurez-vous que Grid a getRuleSet()
            int next_rules_int = (static_cast<int>(current_rules) + 1) % static_cast<int>(RuleSet::COUNT);
            grid.setRuleSet(static_cast<RuleSet>(next_rules_int));
        }
    } else if (!wasSelection) {
        // Grid click (not a selection drag or drawing)
        int grid_x = floor((b.x - camera_x) / (CELL_SIZE * zoom));
        int grid_y = floor((b.y - camera_y) / (CELL_SIZE * zoom));
        if (godModeActive) {
            grid.setGodCell(grid_x, grid_y, !grid.isGod(grid_x, grid_y));
        } else {
            grid.setCell(grid_x, grid_y, !grid.isAlive(grid_x, grid_y));
            addToHistory();
        }
    }
    // Reset selection rectangle if it was a selection, and not a drawing action
    if (isSelecting) {
        selectionRect = {0, 0, 0, 0};
    }
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
    if (!paused && current_time > last_update_time + simulation_speed_ms) {
        grid.update(paused);
        generation_count++;
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

    // Stats
    std::string genText = "Generation: " + std::to_string(generation_count);
    renderText(genText.c_str(),  10, 10, 0, 0, textColor);
    
    std::string popText = "Population: " + std::to_string(grid.getAliveCells().size());
    renderText(popText.c_str(), 10, 40, 0, 0, textColor);

    std::string speedText = "Speed: " + std::to_string(simulation_speed_ms) + "ms";
    renderText(speedText.c_str(), WIDTH - UI_WIDTH + 20, 490, 0, 0, textColor);

    RuleSet current_rules = grid.getRuleSet();
    std::string rulesText = "Rules: ";
    if (current_rules == RuleSet::CONWAY) rulesText += "Conway";
    else if (current_rules == RuleSet::HIGHLIFE) rulesText += "HighLife";
    renderText(rulesText.c_str(), WIDTH - UI_WIDTH + 20, 590, 0, 0, textColor);


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

    // Speed buttons
    SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
    SDL_RenderFillRect(renderer, &slowDownButton);
    renderText("-", slowDownButton.x, slowDownButton.y, slowDownButton.w, slowDownButton.h, textColor);
    SDL_RenderFillRect(renderer, &speedUpButton);
    renderText("+", speedUpButton.x, speedUpButton.y, speedUpButton.w, speedUpButton.h, textColor);

    SDL_SetRenderDrawColor(renderer, 80, 80, 180, 255);
    SDL_RenderFillRect(renderer, &changeRulesButton);
    renderText("Change Rules", changeRulesButton.x, changeRulesButton.y, changeRulesButton.w, changeRulesButton.h, textColor);
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
    SDL_Rect dstRect;
    if (w == 0 && h == 0) { // If width and height are 0, just use the given x,y
        dstRect = { x, y, surface->w, surface->h };
    } else { // Otherwise, center it
        dstRect = { x + (w - surface->w) / 2, y + (h - surface->h) / 2, surface->w, surface->h };
    }
    SDL_RenderCopy(renderer, texture, NULL, &dstRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}

void Game::addToHistory() {
    if (history_index < history.size() - 1) {
        history.erase(history.begin() + history_index + 1, history.end());
    }
    history.push_back(grid.getAliveCells());
    if (history.size() > 50) { // Limite l'historique pour ne pas saturer la mémoire
        history.erase(history.begin());
    }
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