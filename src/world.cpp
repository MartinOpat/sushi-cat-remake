#include "world.h"

World::World() {
    gravity = b2Vec2(0.0f, 10.0f);
    world = new b2World(gravity);
}
