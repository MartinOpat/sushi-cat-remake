#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <vector>
#include <iostream>

#include "object.h"

class Obstacle : public Object {
public:
    Obstacle(std::vector<b2Vec2> vertices, b2World* world, b2Vec2 position, b2BodyType type, bool fixedRotation = false, float angVel = 0, float gravityScale = 1.0f, float density = 1.0f, float friction = 0.3f);

    void render(SDL_Renderer* renderer) override;

private:
    b2Body* body;
};

#endif // OBSTACLE_H