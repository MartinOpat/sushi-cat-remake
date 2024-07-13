#ifndef WALL_H
#define WALL_H

#include "object.h"

class Wall : public Object {
public:
    Wall(float width, float height, b2World* world, b2Vec2 position, b2BodyType type = b2_staticBody, float friction = 0);

    void render(SDL_Renderer* renderer) override;
    b2Body* getb2Body() { return body; }

private:
    b2Body* body;
};

#endif // WALL_H