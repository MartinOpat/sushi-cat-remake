#include "menu.h"

Menu::Menu(bool *running) {
    this->running = running;
    view = new MainView();

    // ----------------- Set up buttons -----------------
    exitButton = new Button(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 + 50, buttonWidth, buttonHeight, "Exit");
    exitButton->bindAction([this](){*(this->running) = false; inMenu = false;});

    instructionsButton = new Button(WINDOW_WIDTH / 2 - 50, WINDOW_HEIGHT / 2 - 50, buttonWidth, buttonHeight, "Controls");
    instructionsButton->bindAction([this](){instructionsDisplayed = !instructionsDisplayed;});

    // ----------------- Set up level picker -----------------
    for (int i = 1; i <= numLevels; i++) {
        Button *levelButton = new Button(WINDOW_WIDTH / 2 - 50 + (i-1-numLevels/2)*1.1*buttonWidth, WINDOW_HEIGHT / 2 - 3*buttonHeight, buttonWidth, buttonHeight, "Level " + std::to_string(i));
        levelButton->bindAction([this, i](){selectedLevel = i; inMenu = false;});
        levelButtons.push_back(levelButton);
    }
}

Menu::~Menu() {
    delete view;
    delete exitButton;
    delete instructionsButton;

    for (Button* button : levelButtons) {
        delete button;
    }
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

// ----------------- Events -----------------

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
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_ESCAPE:
                        instructionsDisplayed = false;
                        break;
                    default:
                        break;
                }
                break;
            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    int newWidth = event.window.data1;
                    int newHeight = event.window.data2;
                    X_WINDOW_SCALE = (float)WINDOW_WIDTH / newWidth;
                    Y_WINDOW_SCALE = (float)WINDOW_HEIGHT / newHeight;
                }
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
    SDL_SetRenderDrawColor(view->getRenderer(), 201, 147, 212, 255);
    SDL_RenderClear(view->getRenderer());

    renderExitButton();
    renderInstructionsButton();
    renderLevelPicker();
    renderTitle();
    renderInstructions();

    SDL_RenderPresent(view->getRenderer());
}

void Menu::renderLevelPicker() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    for (Button* button : levelButtons) {
        button->render(view->getRenderer(), x, y);
    }
}

void Menu::renderExitButton() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    exitButton->render(view->getRenderer(), x, y);
}

void Menu::renderInstructionsButton() {
    int x, y;
    SDL_GetMouseState(&x, &y);
    instructionsButton->render(view->getRenderer(), x, y);
}

void Menu::renderTitle() {
    TTF_Font* font = TTF_OpenFont("Arial.ttf", 96);
    SDL_Surface* surface = TTF_RenderText_Solid(font, "The Sushi Cat Knock Off", {255, 255, 255, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
    SDL_Rect textRect = {WINDOW_WIDTH / 2 - surface->w / 2, WINDOW_HEIGHT / 4 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}

void Menu::renderInstructions() {
    if (instructionsDisplayed) {
        // Displays an SDL2 rectangle with text instructions
        SDL_Rect rect;

        int margin = 100;
        rect.x = margin; 
        rect.y = margin; 
        rect.w = WINDOW_WIDTH - 2*margin; 
        rect.h = WINDOW_HEIGHT - 2*margin;

        // Enable blending mode
        SDL_SetRenderDrawBlendMode(view->getRenderer(), SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(view->getRenderer(), 241, 195, 208, 240);
        SDL_RenderFillRect(view->getRenderer(), &rect);

        TTF_Font* fontBig = TTF_OpenFont("Arial.ttf", 96);
        TTF_Font* fontSmall = TTF_OpenFont("Arial.ttf", 48);

        // Display title
        SDL_Surface* surface = TTF_RenderText_Solid(fontBig, "Controls:", {0, 0, 0, 255});
        SDL_Texture* texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        SDL_Rect textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);


        // Hold and drag mouse to move cat
        surface = TTF_RenderText_Solid(fontSmall, "Hold and drag mouse to move cat", {0, 0, 0, 255});
        texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4 + 2*margin, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Release mouse to drop cat
        surface = TTF_RenderText_Solid(fontSmall, "Release mouse to drop cat", {0, 0, 0, 255});
        texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4 + 3*margin, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Press 'space' to reset cat
        surface = TTF_RenderText_Solid(fontSmall, "Press 'space' to reset cat", {0, 0, 0, 255});
        texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4 + 4*margin, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // Press the exit button to exit level and go back to menu
        surface = TTF_RenderText_Solid(fontSmall, "Press the exit button (or 'esc') to exit level and go back to menu", {0, 0, 0, 255});
        texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4 + 5*margin, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        // To close this window, press 'esc'
        surface = TTF_RenderText_Solid(fontSmall, "To close this window, press 'esc'", {0, 0, 0, 255});
        texture = SDL_CreateTextureFromSurface(view->getRenderer(), surface);
        textRect = {(WINDOW_WIDTH - surface->w)/2, (WINDOW_HEIGHT - surface->h)/4 + 6*margin, surface->w, surface->h};
        SDL_RenderCopy(view->getRenderer(), texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);

        TTF_CloseFont(fontBig);
        TTF_CloseFont(fontSmall);
    }
}

