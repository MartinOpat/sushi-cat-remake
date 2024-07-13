#ifndef WORLD_H
#define WORLD_H

#include "../box2d-main/include/box2d/box2d.h"

class World {
public:
    World();

    b2World* getb2World() { return world; }

private:
    b2Vec2 gravity;
    b2World *world;
};

#endif // WORLD_H