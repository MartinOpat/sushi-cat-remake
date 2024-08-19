#include "mainview.h"

MainView::MainView() {
    // ----------------- Initialize SDL -----------------
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    window = SDL_CreateWindow("Box2D and SDL2 Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH / X_WINDOW_SCALE, WINDOW_HEIGHT / Y_WINDOW_SCALE, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // #ifdef __WIN32__
    //     SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    // #endif

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
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    std::string text = std::to_string(sushisLeft) + " sushi left";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = (WINDOW_WIDTH) / X_LEVEL_WINDOW_SCALE  - surface->w - 10;
    rect.y = 10 / Y_LEVEL_WINDOW_SCALE;
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
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 56);
    std::string text = std::to_string(sushisEaten) + " sushi eaten";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = (WINDOW_WIDTH/2 - surface->w/2) / X_LEVEL_WINDOW_SCALE;
    rect.y = (WINDOW_HEIGHT/4 - surface->h/2) / Y_LEVEL_WINDOW_SCALE;
    rect.w = surface->w / X_LEVEL_WINDOW_SCALE;
    rect.h = surface->h / Y_LEVEL_WINDOW_SCALE;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderLevelWonUI() {
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 64);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Full belly achieved!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = (WINDOW_WIDTH/2 - surface->w/2) / X_LEVEL_WINDOW_SCALE;
    rect.y = (WINDOW_HEIGHT/2 - surface->h/2) / Y_LEVEL_WINDOW_SCALE;
    rect.w = surface->w / X_LEVEL_WINDOW_SCALE;
    rect.h = surface->h / Y_LEVEL_WINDOW_SCALE;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void MainView::renderLevelLostUI() {
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 64);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "Still hungry!", color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = (WINDOW_WIDTH/2 - surface->w/2) / X_LEVEL_WINDOW_SCALE;
    rect.y = (WINDOW_HEIGHT/2 - surface->h/2) / Y_LEVEL_WINDOW_SCALE;
    rect.w = surface->w / X_LEVEL_WINDOW_SCALE;
    rect.h = surface->h / Y_LEVEL_WINDOW_SCALE;

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

void MainView::renderBoxes() {
    // Draw 4 boxes at the bottom of the screen using the bgBoxTexture
    if (isBoxLoaded) {
        SDL_Rect rect;
        int offset = BOX_OFFSET;
        rect.x = -offset;
        rect.y = WINDOW_HEIGHT*0.9f;
        rect.w = WINDOW_WIDTH/4 + 2*offset;
        rect.h = 0.1f*WINDOW_HEIGHT;
        SDL_RenderCopy(renderer, bgBoxTexture, NULL, &rect);

        rect.x = WINDOW_WIDTH/4 - offset;
        SDL_RenderCopy(renderer, bgBoxTexture, NULL, &rect);

        rect.x = WINDOW_WIDTH/2 - offset;
        SDL_RenderCopy(renderer, bgBoxTexture, NULL, &rect);

        rect.x = 3*WINDOW_WIDTH/4 - offset;
        SDL_RenderCopy(renderer, bgBoxTexture, NULL, &rect);
    } else {
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_Rect rect;
        rect.x = 0;
        rect.y = WINDOW_HEIGHT - 100;
        rect.w = 100;
        rect.h = 100;
        SDL_RenderFillRect(renderer, &rect);

        rect.x = WINDOW_WIDTH/4 - 50;
        SDL_RenderFillRect(renderer, &rect);

        rect.x = WINDOW_WIDTH/2 - 50;
        SDL_RenderFillRect(renderer, &rect);

        rect.x = 3*WINDOW_WIDTH/4 - 50;
        SDL_RenderFillRect(renderer, &rect);
    }
}

void MainView::renderCatsLeftUI(int catsLeft) {
    // Small number in the top right corner
    SDL_Color color = {255, 255, 255, 255};
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    std::string text = "Cats: " + std::to_string(catsLeft) + " x";
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), color);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = 10 / X_LEVEL_WINDOW_SCALE;
    rect.y = 10 / Y_LEVEL_WINDOW_SCALE;
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

void MainView::setBoxTexture(std::string boxTex) {
    bgBoxTexture = IMG_LoadTexture(renderer, boxTex.c_str());
    isBoxLoaded = true;
}
