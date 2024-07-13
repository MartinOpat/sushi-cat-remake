#ifndef OBJECT_H
#define OBJECT_H

#include "../box2d-main/include/box2d/box2d.h"
#include "globals.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>


class Object {
public:
    Object(b2Vec2 position, b2BodyType type, bool fixedRotation = false, float angVel = 0, float gravityScale = 1.0f);

    virtual void render(SDL_Renderer* renderer) = 0;

protected:
    b2BodyDef bodyDef;
};

#endif // OBJECT_H