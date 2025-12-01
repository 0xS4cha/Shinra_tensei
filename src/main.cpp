#include <SDL2/SDL.h>
#include <vector>

const int CELL_SIZE = 20;
int x_pos = 0;
int y_pos = 0;

int countNeighbors(const std::vector<std::vector<int>>& grid, int x, int y, int ROWS, int COLS) {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            if (nx >= 0 && nx < COLS && ny >= 0 && ny < ROWS)
                count += grid[ny][nx];
        }
    }
    return count;
}

void iterate(std::vector<std::vector<int>>& grid, int ROWS, int COLS) {
    std::vector<std::vector<int>> newGrid = grid;
    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLS; ++x) {
            int n = countNeighbors(grid, x, y, ROWS, COLS);
            if (grid[y][x]) {
                if (n < 2 || n > 3) newGrid[y][x] = 0;
            } else {
                if (n == 3) newGrid[y][x] = 1;
            }
        }
    }
    grid = newGrid;
}

int main() {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    const int WIDTH = dm.w;
    const int HEIGHT = dm.h;
    const int COLS = WIDTH / CELL_SIZE;
    const int ROWS = HEIGHT / CELL_SIZE;

    SDL_Window* mainWindow = SDL_CreateWindow("Game of Life",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WIDTH, HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);

    SDL_Window* controlWindow = SDL_CreateWindow("Controls",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        200, 100, SDL_WINDOW_SHOWN);
    SDL_Renderer* controlRenderer = SDL_CreateRenderer(controlWindow, -1, SDL_RENDERER_ACCELERATED);

    std::vector<std::vector<int>> grid(ROWS, std::vector<int>(COLS, 0));

    SDL_Rect skipButton = { 20, 20, 70, 40 };
    SDL_Rect preSkipButton = { 110, 20, 70, 40 };

    int x_pos = 0;
    int y_pos = 0;

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                running = false;
            else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_LEFT)
                    x_pos -= CELL_SIZE;
                else if (event.key.keysym.sym == SDLK_RIGHT)
                    x_pos += CELL_SIZE;
                else if (event.key.keysym.sym == SDLK_UP)
                    y_pos -= CELL_SIZE;
                else if (event.key.keysym.sym == SDLK_DOWN)
                    y_pos += CELL_SIZE;
            }
            else if (event.type == SDL_MOUSEBUTTONDOWN) {
                if (event.window.windowID == SDL_GetWindowID(mainWindow)) {
                    int x = (event.button.x - x_pos) / CELL_SIZE;
                    int y = (event.button.y - y_pos) / CELL_SIZE;
                    if (x >= 0 && x < COLS && y >= 0 && y < ROWS)
                        grid[y][x] = !grid[y][x]; // toggle cellule
                } else if (event.window.windowID == SDL_GetWindowID(controlWindow)) {
                    int mx = event.button.x;
                    int my = event.button.y;
                    if (mx >= skipButton.x && mx <= skipButton.x + skipButton.w &&
                        my >= skipButton.y && my <= skipButton.y + skipButton.h) {
                        iterate(grid, ROWS, COLS);
                    } else if (mx >= preSkipButton.x && mx <= preSkipButton.x + preSkipButton.w &&
                               my >= preSkipButton.y && my <= preSkipButton.y + preSkipButton.h) {
                        // PreSkip : à implémenter si tu veux un historique
                    }
                }
            }
        }

        // Dessin de la grille principale
        SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);
        SDL_RenderClear(mainRenderer);

        SDL_SetRenderDrawColor(mainRenderer, 180, 180, 180, 255);
        for (int y = 0; y <= ROWS; ++y)
            SDL_RenderDrawLine(mainRenderer, 0, y * CELL_SIZE + y_pos, WIDTH, y * CELL_SIZE + y_pos);
        for (int x = 0; x <= COLS; ++x)
            SDL_RenderDrawLine(mainRenderer, x * CELL_SIZE + x_pos, 0, x * CELL_SIZE + x_pos, HEIGHT);

        SDL_SetRenderDrawColor(mainRenderer, 0, 255, 0, 255);
        for (int y = 0; y < ROWS; ++y) {
            for (int x = 0; x < COLS; ++x) {
                if (grid[y][x]) {
                    SDL_Rect cell = { x * CELL_SIZE + x_pos, y * CELL_SIZE + y_pos, CELL_SIZE, CELL_SIZE };
                    SDL_RenderFillRect(mainRenderer, &cell);
                }
            }
        }

        SDL_RenderPresent(mainRenderer);

        // Dessin fenêtre de contrôle
        SDL_SetRenderDrawColor(controlRenderer, 50, 50, 50, 255);
        SDL_RenderClear(controlRenderer);

        SDL_SetRenderDrawColor(controlRenderer, 0, 0, 255, 255);
        SDL_RenderFillRect(controlRenderer, &skipButton);

        SDL_SetRenderDrawColor(controlRenderer, 255, 0, 0, 255);
        SDL_RenderFillRect(controlRenderer, &preSkipButton);

        SDL_RenderPresent(controlRenderer);

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(mainRenderer);
    SDL_DestroyWindow(mainWindow);
    SDL_DestroyRenderer(controlRenderer);
    SDL_DestroyWindow(controlWindow);
    SDL_Quit();

    return 0;
}
