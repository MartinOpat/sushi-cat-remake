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

void Button::render(SDL_Renderer* renderer) {
    SDL_Rect rect = {x, y, width, height};
    SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderDrawRect(renderer, &rect);

    TTF_Font* font = TTF_OpenFont("Arial.ttf", 24);
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), {255, 255, 255, 255});
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_Rect textRect = {x + width / 2 - surface->w / 2, y + height / 2 - surface->h / 2, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &textRect);
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}
