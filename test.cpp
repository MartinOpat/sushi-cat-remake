#include <SDL.h>
#include <SDL_ttf.h>
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

// Constants
const int WIDTH = 500, HEIGHT = 600;
const int ROWS = 6, COLS = 5;
const int CELL_SIZE = WIDTH / COLS;
const int CAT_RADIUS = CELL_SIZE / 4;
const int SUSHI_RADIUS = CAT_RADIUS / 2;
const int GRAVITY = 1;

// Function declarations
void initSDL(SDL_Window** window, SDL_Renderer** renderer);
void closeSDL(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font);
void drawCircle(SDL_Renderer* renderer, int x, int y, int radius);
void drawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y);
SDL_Rect createRect(int x, int y, int w, int h);

int main(int argc, char* args[]) {
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font* font = nullptr;

    initSDL(&window, &renderer);
    font = TTF_OpenFont("Arial.ttf", 36);  // Ensure you have this font or specify a path to a valid font file
    if (font == nullptr) {
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        closeSDL(window, renderer, font);
        return -1;
    }

    srand(static_cast<unsigned int>(time(nullptr)));

    // Generate sushi positions
    std::vector<SDL_Point> sushi_positions;
    for (int i = 0; i < 15; ++i) {
        int x = rand() % COLS * CELL_SIZE + CELL_SIZE / 2;
        int y = rand() % ROWS * CELL_SIZE + CELL_SIZE / 2;
        sushi_positions.push_back({x, y});
    }

    // Generate obstacles
    std::vector<SDL_Rect> obstacles;
    for (int col = 0; col < COLS; ++col) {
        for (int row = 0; row < ROWS; ++row) {
            if (rand() % 100 < 15) { // 15% chance of obstacle
                int x = col * CELL_SIZE;
                int y = row * CELL_SIZE + CELL_SIZE / 2;
                int width = CELL_SIZE;
                int height = CELL_SIZE / 3;
                obstacles.push_back(createRect(x, y, width, height));
            }
        }
    }

    bool running = true;
    SDL_Event e;
    int cat_x = -1, cat_y = 0;
    int velocity_y = 0, velocity_x = 0;
    bool mouse_held_down = false;
    int sushi_eaten = 0;

    while (running) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                running = false;
            } else if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                SDL_GetMouseState(&cat_x, &cat_y);
                mouse_held_down = true;
            } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
                mouse_held_down = false;
                velocity_y = GRAVITY;
                velocity_x = 0;
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);  // White background
        SDL_RenderClear(renderer);

        // Draw sushi
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Green
        for (const auto& sushi : sushi_positions) {
            drawCircle(renderer, sushi.x, sushi.y, SUSHI_RADIUS);
        }

        // Draw obstacles
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red
        for (const auto& obstacle : obstacles) {
            SDL_RenderFillRect(renderer, &obstacle);
        }

        // Update and draw cat
        if (cat_x != -1) {
            if (mouse_held_down) {
                SDL_GetMouseState(&cat_x, &cat_y);
                cat_y = std::max(cat_y, 0);
            } else {
                cat_y += velocity_y;
                velocity_y += GRAVITY;
                cat_x += velocity_x;

                // Collision with the bottom of the window
                if (cat_y > HEIGHT - CAT_RADIUS) {
                    cat_y = HEIGHT - CAT_RADIUS;
                    velocity_y = 0;
                    velocity_x = 0;
                }

                // Collision with obstacles
                SDL_Rect catRect = createRect(cat_x - CAT_RADIUS, cat_y - CAT_RADIUS, 2 * CAT_RADIUS, 2 * CAT_RADIUS);
                for (const auto& obstacle : obstacles) {
                    if (SDL_HasIntersection(&obstacle, &catRect)) {
                        cat_y = obstacle.y - CAT_RADIUS;
                        velocity_y = 0;
                        break;
                    }
                }

                // Collect sushi
                std::vector<SDL_Point> remaining_sushi;
                for (const auto& sushi : sushi_positions) {
                    int dx = cat_x - sushi.x;
                    int dy = cat_y - sushi.y;
                    if (sqrt(dx * dx + dy * dy) < (CAT_RADIUS + SUSHI_RADIUS)) {
                        sushi_eaten++;
                    } else {
                        remaining_sushi.push_back(sushi);
                    }
                }
                sushi_positions = remaining_sushi;
            }

            // Draw cat
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black
            drawCircle(renderer, cat_x, cat_y, CAT_RADIUS);
        }

        // Display score
        std::string scoreText = "Sushi Eaten: " + std::to_string(sushi_eaten);
        drawText(renderer, font, scoreText, 10, 10);

        SDL_RenderPresent(renderer);
        SDL_Delay(1000 / 60);  // Approximately 60 FPS
    }

    closeSDL(window, renderer, font);
    return 0;
}

void initSDL(SDL_Window** window, SDL_Renderer** renderer) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (TTF_Init() == -1) {
        std::cerr << "SDL_ttf could not initialize! TTF_Error: " << TTF_GetError() << std::endl;
        exit(1);
    }

    *window = SDL_CreateWindow("Sushi Cat", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (*window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (*renderer == nullptr) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void closeSDL(SDL_Window* window, SDL_Renderer* renderer, TTF_Font* font) {
    if (font != nullptr) TTF_CloseFont(font);
    if (renderer != nullptr) SDL_DestroyRenderer(renderer);
    if (window != nullptr) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

void drawCircle(SDL_Renderer* renderer, int x, int y, int radius) {
    for (int w = 0; w < radius * 2; w++) {
        for (int h = 0; h < radius * 2; h++) {
            int dx = radius - w;  // horizontal offset
            int dy = radius - h;  // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                SDL_RenderDrawPoint(renderer, x + dx, y + dy);
            }
        }
    }
}


void drawText(SDL_Renderer* renderer, TTF_Font* font, std::string text, int x, int y) {
    SDL_Color black = {0, 0, 0, 255};
    SDL_Surface* surfaceMessage = TTF_RenderText_Solid(font, text.c_str(), black);
    SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
    SDL_Rect message_rect;
    message_rect.x = x;
    message_rect.y = y;
    message_rect.w = surfaceMessage->w;
    message_rect.h = surfaceMessage->h;

    SDL_RenderCopy(renderer, message, NULL, &message_rect);

    SDL_FreeSurface(surfaceMessage);
    SDL_DestroyTexture(message);
}

SDL_Rect createRect(int x, int y, int w, int h) {
    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = w;
    rect.h = h;
    return rect;
}
