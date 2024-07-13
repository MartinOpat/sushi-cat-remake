#ifndef MAINAPP_H
#define MAINAPP_H

#include "globals.h"
#include "mainview.h"
#include "world.h"
#include "wall.h"

#include "../box2d-main/include/box2d/box2d.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <vector>
#include <chrono>

class MainApp {
public:
    MainApp();
    ~MainApp();

    void run();

private:
    // ----------------- Physics Objects -----------------
    MainView *view;
    World *world;
    std::vector<Wall*> walls;

    // ----------------- Control -----------------
    bool quit = false;
    void advanceTime();

    // ----------------- Rendering -----------------
    void render();
    void renderWalls();

};

#endif // MAINAPP_H