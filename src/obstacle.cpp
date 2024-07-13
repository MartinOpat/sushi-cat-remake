#include "obstacle.h"

Obstacle::Obstacle(std::vector<b2Vec2> vertices, b2World* world, b2Vec2 position, b2BodyType type, bool fixedRotation, float angVel, float gravityScale, float density, float friction) : Object(position, type, fixedRotation, angVel, gravityScale) {
    // ------------------- Create the body -------------------
    b2PolygonShape shape;
    shape.Set(vertices.data(), vertices.size());

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &shape;
    fixtureDef.density = density;
    fixtureDef.friction = friction;

    // bodyDef from super
    body = world->CreateBody(&bodyDef);
    body->CreateFixture(&fixtureDef);
}

void Obstacle::render(SDL_Renderer* renderer) {
    // Render an obstacle as a polygon to allow rotation
    b2Fixture* fixture = body->GetFixtureList();
    while (fixture) {
        b2Shape* shape = fixture->GetShape();
        if (shape->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* polygon = static_cast<b2PolygonShape*>(shape);
            const int32 vertexCount = polygon->m_count;
            Sint16 vx[vertexCount];
            Sint16 vy[vertexCount];

            for (int i = 0; i < vertexCount; ++i) {
                b2Vec2 wp = body->GetWorldPoint(polygon->m_vertices[i]);
                vx[i] = static_cast<Sint16>(wp.x * SCALE);
                vy[i] = static_cast<Sint16>(wp.y * SCALE);
            }

            // Draw filled polygon with rotation handled
            filledPolygonRGBA(renderer, vx, vy, vertexCount, 0, 0, 0, 255);
        
        }
        fixture = fixture->GetNext();
    }
}