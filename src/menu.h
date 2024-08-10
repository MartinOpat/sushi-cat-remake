#ifndef MENU_H
#define MENU_H

#include "globals.h"
#include <SDL2/SDL.h>

#include "button.h"
#include "mainview.h"

class Menu {
public:
    Menu(bool *running);
    ~Menu();

    int run();

    MainView *view;

    bool inMenu = true;

private:
    // ----------------- Control -----------------
    bool *running;

    // ----------------- Rendering -----------------
    void renderLevelPicker();
    void renderExitButton();
    void renderTitle();
    void renderInstructionsButton();

    void render();

    // ----------------- Events -----------------
    void handleEvents();
    void handleMouseClick(int x, int y);

    // ----------------- Buttons -----------------
    Button *exitButton = nullptr;
    Button *instructionsButton = nullptr;

    // ----------------- Level Picker -----------------
    int selectedLevel = 1;  // default


};

#endif // MENU_H