#include <iostream>

#define SDL_MAIN_HANDLED    // Tell SDL that we'll set up our own main function
#include <SDL2/SDL.h>
#include <unistd.h>

#include "globals.h"
#include "mainapp.h"
#include "menu.h"

// ----------------- Globals -----------------
int WINDOW_WIDTH = 1920;
int WINDOW_HEIGHT = 1080;

float PHYSICS_SCALE = 50.0f;
float X_WINDOW_SCALE = 1.0f;
float Y_WINDOW_SCALE = 1.0f;
float X_LEVEL_WINDOW_SCALE = 1.0f;
float Y_LEVEL_WINDOW_SCALE = 1.0f;

int NUM_CAT_PARTICLES = 25;
int BASE_CAT_RADIUS = 1.0f;
int CAT_SPAWN_HEIGHT = 2*BASE_CAT_RADIUS;

int SUSHI_WIDTH = 0.5f;
int SUSHI_HEIGHT = 0.5f;

int BOX_OFFSET = 50;
// -------------------------------------------

int main() {
    bool running = true;

    MainApp *app = nullptr;
    while (running) {
        // Open menu with level picker
        std::cout << "Opening menu" << std::endl;
        Menu *menu = new Menu(&running);
        int level = menu->run();
        delete menu;

        // Check if we should exit
        if (!running) {
            std::cout << "Exiting" << std::endl;
            break;
        }
        std::cout << "Level selected: " << level << std::endl;

        // Set window scale
        X_LEVEL_WINDOW_SCALE = X_WINDOW_SCALE;
        Y_LEVEL_WINDOW_SCALE = Y_WINDOW_SCALE;

        // Open main app with selected level
        app = new MainApp(level);
        app->run();
        delete app;
        app = nullptr;
    }
    if (app != nullptr)
        delete app;

    return 0;
}