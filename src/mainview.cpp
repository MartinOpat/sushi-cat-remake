#include "mainview.h"

MainView::MainView() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    // IMG_Init(IMG_INIT_PNG);ma

    window = SDL_CreateWindow("Box2D and SDL2 Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

MainView::~MainView() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
