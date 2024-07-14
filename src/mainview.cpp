#include "mainview.h"

MainView::MainView() {
    // ----------------- Initialize SDL -----------------
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    window = SDL_CreateWindow("Box2D and SDL2 Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // ----------------- Load font -----------------
    TTF_Init();
}

MainView::~MainView() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    TTF_Quit();
}

void MainView::renderSushiLeftUI(int sushisLeft) {
    // Small number in the top right corner
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 24);
    std::string text = std::to_string(sushisLeft) + " sushi left";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = WINDOW_WIDTH - surface->w - 10;
    rect.y = 10;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderSushiEatenUI(int sushisEaten) {
    // Display how many sushies were eaten like: "x sushi eaten" onto upper center of the screen
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 56);
    std::string text = std::to_string(sushisEaten) + " sushi eaten";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = WINDOW_WIDTH/2 - surface->w/2;
    rect.y = WINDOW_HEIGHT/4 - surface->h/2;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderLevelWonUI() {
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 64);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Full belly achieved!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = WINDOW_WIDTH/2 - surface->w/2;
    rect.y = WINDOW_HEIGHT/2 - surface->h/2;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderLevelLostUI() {
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 64);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Still hungry!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = WINDOW_WIDTH/2 - surface->w/2;
    rect.y = WINDOW_HEIGHT/2 - surface->h/2;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderBackground() {
    if (isBgLoaded) {
        SDL_RenderCopy(renderer, bgTexture, NULL, NULL);
    } else {
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        SDL_RenderClear(renderer);
    }

}

void MainView::renderCatsLeftUI(int catsLeft) {
    // Small number in the top right corner
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/fonts-japanese-gothic.ttf", 24);
    std::string text = "Cats: " + std::to_string(catsLeft) + " x";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = 10;
    rect.y = 10;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::setBackground(std::string bgTex) {
    bgTexture = IMG_LoadTexture(renderer, bgTex.c_str());
    isBgLoaded = true;
}
