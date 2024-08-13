#ifndef BUTTON_H
#define BUTTON_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <functional>
#include <iostream>

class Button {
public:

    Button(int x, int y, int width, int height, std::string text);

    bool isWithinBounds(int x, int y);
    
    // This should accept lambdas
    void bindAction(std::function<void()> action) {this->action = action;}

    void doClick() {action();std::cout << "Button clicked" << std::endl;}

    void render(SDL_Renderer *renderer, int mouseX, int mouseY);

    
private:
    int x;
    int y;
    int width;
    int height;
    std::string text;

    SDL_Color bgColor = {0, 0, 0, 255};
    SDL_Color bgBaseColor = {0, 0, 0, 255};
    SDL_Color bgHoverColor = {50, 50, 50, 255};


    std::function<void()> action;

};

#endif // BUTTON_H