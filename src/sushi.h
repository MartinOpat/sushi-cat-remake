#ifndef SUSHI_H
#define SUSHI_H

#include "object.h"

class Sushi : public Object {
public:
    Sushi(b2Vec2 position, b2World* world);

    void render(SDL_Renderer* renderer) override;

private:
    b2Body* body;
};

#endif // SUSHI_H