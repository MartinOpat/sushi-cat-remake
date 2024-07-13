#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "globals.h"

class MainView {
public:
    MainView();
    ~MainView();

    SDL_Renderer* getRenderer() { return renderer; }
    SDL_Window* getWindow() { return window; }
    
    void renderSushiLeftUI(int sushisLeft);
    void renderSushiEatenUI(int sushisEaten);
    void renderLevelWonUI();

private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif // MAINVIEW_H