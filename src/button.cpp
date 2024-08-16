#include "button.h"

Button::Button(int x, int y, int width, int height, std::string text) {
    this->x = x;
    this->y = y;
    this->width = width;
    this->height = height;
    this->text = text;
}

bool Button::isWithinBounds(int x, int y) {
    return x >= this->x && x <= this->x + this->width && y >= this->y && y <= this->y + this->height;
}

void Button::render(SDL_Renderer* renderer, int mouseX, int mouseY) {
    if (isWithinBounds(mouseX, mouseY)) {
        bgColor = bgHoverColor;
    } else {
        bgColor = bgBaseColor;
    }
    
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    if (font == nullptr) {
        // Handle the error, for example, log it and return
        std::cerr << "Failed to load font: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255, 255});
    if (surface == nullptr) {
        // Handle the error, for example, log it and return
        std::cerr << "Failed to create text surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (texture == nullptr) {
        // Handle the error, for example, log it and return
        std::cerr << "Failed to create texture from surface: " << SDL_GetError() << std::endl;
        return;
    }

    SDL_Rect textRect = {x + width / 2 - surface->w / 2, y + height / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(font);
}
