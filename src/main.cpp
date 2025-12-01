#include <SDL2/SDL.h>
#include <vector>

const int CELL_SIZE = 20;

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    
    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);

    const int WIDTH = dm.w;
    const int HEIGHT = dm.h;
    const int COLS = (WIDTH / CELL_SIZE);
    const int ROWS = HEIGHT / CELL_SIZE;

    SDL_Window* window = SDL_CreateWindow("Window",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Grille d'état des cellules (0 = vide, 1 = activé)
    std::vector<std::vector<int>> grid(ROWS, std::vector<int>(COLS, 0));

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x / CELL_SIZE;
                int y = event.button.y / CELL_SIZE;
                if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
                    grid[y][x] = !grid[y][x]; // toggle cellule
            }
        }

        // Fond noir
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // Dessiner la grille
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // lignes blanches
        for (int y = 0; y <= ROWS; ++y) {
            SDL_RenderDrawLine(renderer, 0, y * CELL_SIZE, WIDTH, y * CELL_SIZE);
        }
        for (int x = 0; x <= COLS; ++x) {
            SDL_RenderDrawLine(renderer, x * CELL_SIZE, 0, x * CELL_SIZE, HEIGHT);
        }

        // Dessiner les cellules activées (remplies)
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // vert
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (grid[y][x]) {
                    SDL_Rect cell = { x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
                    SDL_RenderFillRect(renderer, &cell);
                }
            }
        }

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
