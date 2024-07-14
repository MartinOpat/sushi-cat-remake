#ifndef SUSHI_H
#define SUSHI_H

#include "object.h"
#include "globals.h"

#include <iostream>
#include <vector>

class Sushi : public Object {
public:
    Sushi(b2Vec2 position, b2World* world);
    ~Sushi() {SDL_DestroyTexture(sushiTex);};

    void render(SDL_Renderer* renderer) override;
    b2Body* getb2Body() {return body;};

private:
    // ------------------- Physics Objects -------------------
    b2Body* body;

    // ------------------- Textures -------------------
    void loadTextures(SDL_Renderer* renderer);

    SDL_Texture *sushiTex = nullptr;
};

#endif // SUSHI_H