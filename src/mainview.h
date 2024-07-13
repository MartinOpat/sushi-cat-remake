#ifndef MAINVIEW_H
#define MAINVIEW_H

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>

#include "globals.h"

class MainView {
public:
    MainView();
    
private:
    SDL_Window* window;
    SDL_Renderer* renderer;
};

#endif // MAINVIEW_H