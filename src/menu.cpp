#include "menu.h"

Menu::Menu(bool *running) {
    this->running = running;
    view = new MainView();

    // Set up buttons
    exitButton = new Button(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 50, buttonWidth, buttonHeight, "Exit");
    exitButton->bindAction([this](){*(this->running) = false; inMenu = false;});

    instructionsButton = new Button(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 50, buttonWidth, buttonHeight, "Controls");
    // TODO: Actually add a pop-up window with instructions

    // Set up level picker
    for (int i = 1; i <= 5; i++) {
        Button *levelButton = new Button(WINDOW_WIDTH / 2 - 50 + (i-1)*1.1*buttonWidth, WINDOW_HEIGHT / 2 - 3*buttonHeight, buttonWidth, buttonHeight, "Level " + std::to_string(i));
        levelButton->bindAction([this, i](){selectedLevel = i; inMenu = false;});
        levelButtons.push_back(levelButton);
    }
}

Menu::~Menu() {
    delete view;
    delete exitButton;
    delete instructionsButton;
}

int Menu::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // Milliseconds per frame
    Uint32 frameStart;
    int frameTime;

    while (inMenu) {
        frameStart = SDL_GetTicks();

        handleEvents();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }

    }

    return selectedLevel;
}

void Menu::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                *running = false;
                inMenu = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                int x, y;
                SDL_GetMouseState(&x, &y);
                std::cout << "Mouse click at (" << x << ", " << y << ")" << std::endl;
                handleMouseClick(x, y);
                break;
            default:
                break;
        }
    }
}

void Menu::handleMouseClick(int x, int y) {
    if (exitButton->isWithinBounds(x, y)) {
        exitButton->doClick();
    } else if (instructionsButton->isWithinBounds(x, y)) {
        instructionsButton->doClick();
    } else {
        for (Button* button : levelButtons) {
            if (button->isWithinBounds(x, y)) {
                button->doClick();
            }
        }
    }
}

// ----------------- Rendering -----------------

void Menu::render() {
    SDL_SetRenderDrawColor(view->getRenderer(), 100, 100, 100, 255);
    SDL_RenderClear(view->getRenderer());

    renderExitButton();
    renderInstructionsButton();
    renderLevelPicker();

    SDL_RenderPresent(view->getRenderer());
}

void Menu::renderLevelPicker() {
    for (Button* button : levelButtons) {
        button->render(view->getRenderer());
    }
}

void Menu::renderExitButton() {
    exitButton->render(view->getRenderer());
}

void Menu::renderInstructionsButton() {
    instructionsButton->render(view->getRenderer());
}

void Menu::renderTitle() {
}


