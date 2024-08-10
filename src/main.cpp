#include <iostream>

#define SDL_MAIN_HANDLED    // Tell SDL that we'll set up our own main function
#include <SDL2/SDL.h>

#include "globals.h"
#include "mainapp.h"

// ----------------- Globals -----------------
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;
float SCALE = 50.0f;

int NUM_CAT_PARTICLES = 25;
int BASE_CAT_RADIUS = 1.0f;
int CAT_SPAWN_HEIGHT = 2*BASE_CAT_RADIUS;

int SUSHI_WIDTH = 0.5f;
int SUSHI_HEIGHT = 0.5f;

int BOX_OFFSET = 50;
// -------------------------------------------

int main() {
    MainApp app;
    app.run();

    return 0;
}