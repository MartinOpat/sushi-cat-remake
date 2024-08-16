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
float SCALE = 50.0f;

int NUM_CAT_PARTICLES = 25;
int BASE_CAT_RADIUS = 1.0f;
int CAT_SPAWN_HEIGHT = 2*BASE_CAT_RADIUS;

int SUSHI_WIDTH = 0.5f;
int SUSHI_HEIGHT = 0.5f;

int BOX_OFFSET = 50;
// -------------------------------------------

int main() {
    bool running = true;

    while (running) {
        // Open menu with level picker
        std::cout << "Opening menu" << std::endl;
        Menu *menu = new Menu(&running);
        int level = menu->run();
        delete menu;

        if (!running) {
            std::cout << "Exiting" << std::endl;
            break;
        }
        std::cout << "Level selected: " << level << std::endl;


        MainApp *app = new MainApp(level);
        app->run();
        delete app;
    }

    return 0;
}