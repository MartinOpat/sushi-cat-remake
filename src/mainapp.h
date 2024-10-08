#ifndef MAINAPP_H
#define MAINAPP_H

#include "globals.h"
#include "mainview.h"
#include "world.h"
#include "wall.h"
#include "obstacle.h"
#include "cat.h"
#include "sushi.h"

#include <box2d/box2d.h>
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <vector>
#include <list>
#include <set>
#include <chrono>
#include <algorithm>
#include <lvl.h>

class MainApp {
public:
    MainApp(int level=1);
    ~MainApp();

    void run();

private:
    // ----------------- Physics Objects -----------------
    MainView *view;
    World *world;

    std::vector<Wall*> walls;
    std::vector<Obstacle*> kinematicObstacles;
    std::vector<Obstacle*> staticObstacles;
    std::vector<Obstacle*> dynamicObstacles;
    std::list<Sushi*> sushis;

    // ----------------- Cat -----------------
    Cat *cat;
    bool isDragging = false;
    bool isNewCat = false;
    bool isFirstCat = true;
    int lastCatSushiEaten = 0;
    bool newCatReady = true;
    int catsLeft = 0;

    // ----------------- Control -----------------
    bool quit = false;
    void advanceTime();

    void pollEvents();
    void handleMouseClick(int x, int y);
    void handleKeyPress(SDL_Keycode key);

    // ----------------- Physics -----------------
    void handlePhysics();
    void handleCollisions();
    void handleCatStuck();

    // ----------------- Rendering -----------------
    void render();
    void renderWalls();
    void renderObstacles();
    void renderCat();
    void renderSushis();
    void renderUI();

    // ----------------- Level -----------------
    lvl* currLvl;

    // ----------------- Specific keypresses -----------------
    void handleKeyPressSpace();

};

#endif // MAINAPP_H