#include "sushi.h"

Sushi::Sushi(b2Vec2 position, b2World* world) : Object(position, b2_kinematicBody, true, 0, 0) {
    b2PolygonShape shape;
    shape.SetAsBox(0.5f, 0.5f);

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = 0;
    fixtureDef.friction = 0;

    // bodyDef from super
    body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
}


void Sushi::render(SDL_Renderer* renderer) {
    b2PolygonShape* box = static_cast<b2PolygonShape*>(body->GetFixtureList()->GetShape());
    b2Vec2 vertices[4];
    for (int i = 0; i < 4; ++i) {
        vertices[i] = body->GetWorldPoint(box->m_vertices[i]);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect rect;
    rect.x = (int)(vertices[0].x * SCALE);
    rect.y = (int)(vertices[0].y * SCALE);
    rect.w = (int)((vertices[2].x - vertices[0].x) * SCALE);
    rect.h = (int)((vertices[2].y - vertices[0].y) * SCALE);
    SDL_RenderFillRect(renderer, &rect);
}