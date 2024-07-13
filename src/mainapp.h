#ifndef MAINAPP_H
#define MAINAPP_H

#include "globals.h"
#include "mainview.h"
#include "world.h"
#include "wall.h"
#include "obstacle.h"

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
    std::vector<Obstacle*> obstacles;

    // ----------------- Control -----------------
    bool quit = false;
    void advanceTime();
    void pollEvents();

    // ----------------- Rendering -----------------
    void render();
    void renderWalls();
    void renderObstacles();

};

#endif // MAINAPP_H