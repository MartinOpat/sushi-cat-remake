#include <iostream>

#include "globals.h"
#include "mainapp.h"

// ----------------- Globals -----------------
int WINDOW_WIDTH = 1600;
int WINDOW_HEIGHT = 1200;
float SCALE = 1.0f;

int NUM_CAT_PARTICLES = 25;
int BASE_CAT_RADIUS = 1.0f;

int SUSHI_WIDTH = 0.5f;
int SUSHI_HEIGHT = 0.5f;
// -------------------------------------------

int main() {
    MainApp app;
    app.run();

    return 0;
}