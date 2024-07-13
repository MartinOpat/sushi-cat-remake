#include "wall.h"

Wall::Wall(float width, float height, b2World* world, b2Vec2 position, b2BodyType type, float friction) : Object(position, type) {
    b2PolygonShape shape;
    shape.SetAsBox(width / 2, height / 2);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0;  // immovable
    fixtureDef.friction = friction;

    // bodyDef from super
    body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
}

// Override render function
void Wall::render(SDL_Renderer* renderer) {
    b2PolygonShape* shape = static_cast<b2PolygonShape*>(body->GetFixtureList()->GetShape());
    SDL_Rect rect = box2DToSDL(body->GetPosition(), shape->m_vertices[2].x * 2, shape->m_vertices[2].y * 2);
    SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer, &rect);
}