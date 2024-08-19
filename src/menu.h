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
    void renderInstructions();

    void render();

    // ----------------- Events -----------------
    void handleEvents();
    void handleMouseClick(int x, int y);

    // ----------------- Buttons -----------------
    int buttonWidth = 200;
    int buttonHeight = 50;

    Button *exitButton = nullptr;
    Button *instructionsButton = nullptr;
    Button *upscaleButton = nullptr;
    Button *downscaleButton = nullptr;
    std::vector<Button*> levelButtons;

    bool instructionsDisplayed = false;

    // ----------------- Level Picker -----------------
    int selectedLevel = 1;  // default
    int numLevels = 3;


};

#endif // MENU_H