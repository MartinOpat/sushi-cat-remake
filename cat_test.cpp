#include "box2d-main/include/box2d/box2d.h"
#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL2_gfxPrimitives.h>  // Include SDL2_gfx
#include <SDL2/SDL_image.h> // Include SDL2_image for texture loading
#include <vector>
#include <set>
#include <algorithm>
#include <list>
#include <GL/gl.h>
#include <GL/glu.h>
#include <chrono>

const int WINDOW_WIDTH = 1600;
const int WINDOW_HEIGHT = 1200;
const float SCALE = 50.0; // Scale to display Box2D world in pixels
const int NUM_PARTICLES = 25; // Number of particles in the circle
float RADIUS = 1.0f; // Radius of the circular structure in meters
const float SUSHI_WIDTH = 0.5f;
const float SUSHI_HEIGHT = 0.5f;

// Define multiplaction between a b2Mat33 and a b2Vec3 via *
b2Vec3 operator*(const b2Mat33& A, const b2Vec3& v) {
    return b2Vec3(b2Dot(A.ex, v), b2Dot(A.ey, v), b2Dot(A.ez, v));
}

// Define matrix multiplication via * for b2Mat33
b2Mat33 operator*(const b2Mat33& A, const b2Mat33& B) {
    b2Mat33 result;
    result.ex = A * B.ex;
    result.ey = A * B.ey;
    result.ez = A * B.ez;
    return result;
}

// Function to convert Box2D world coordinates to screen coordinates
SDL_Rect box2DToSDL(const b2Vec2& position, float width, float height) {
    SDL_Rect rect;
    rect.x = (int)(position.x * SCALE - width * SCALE / 2);
    rect.y = (int)(position.y * SCALE - height * SCALE / 2);
    rect.w = (int)(width * SCALE);
    rect.h = (int)(height * SCALE);
    return rect;
}


std::vector<b2Body*> particles;
b2World* worldPtr;

std::list<b2Body*> sushis;
std::vector<b2Body*> obstacles;
std::vector<b2Body*> staticObjects;

std::vector<b2DistanceJoint *> outerJoints;
std::vector<b2DistanceJoint *> innerJoints;



void changeParticleCircleRadius() {
    // Calculate the center of the circle
    float centerX = 0.0f, centerY = 0.0f;
    for (const auto& body : particles) {
        centerX += body->GetPosition().x;
        centerY += body->GetPosition().y;
    }
    centerX /= particles.size();
    centerY /= particles.size();

    // Update each particle's position to the new radius
    for (size_t i = 0; i < particles.size(); ++i) {
        float angle = i * 2 * M_PI / particles.size();
        float x = centerX + RADIUS * cos(angle);
        float y = centerY + RADIUS * sin(angle);
        particles[i]->SetTransform(b2Vec2(x, y), particles[i]->GetAngle());
    }

        // Update outer joints
    for (size_t i = 0; i < outerJoints.size(); ++i) {
        b2Body* bodyA = outerJoints[i]->GetBodyA();
        b2Body* bodyB = outerJoints[i]->GetBodyB();
        b2Vec2 posA = bodyA->GetPosition();
        b2Vec2 posB = bodyB->GetPosition();
        float newLength = b2Distance(posA, posB);
        outerJoints[i]->SetLength(newLength);
    }

    // Update inner joints
    for (size_t i = 0; i < innerJoints.size(); ++i) {
        b2Body* bodyA = innerJoints[i]->GetBodyA();
        b2Body* bodyB = innerJoints[i]->GetBodyB();
        b2Vec2 posA = bodyA->GetPosition();
        b2Vec2 posB = bodyB->GetPosition();
        float newLength = b2Distance(posA, posB);
        innerJoints[i]->SetLength(newLength);

        // // Update stiffnes from new frequency -> Kinda works, not the best
        // float f = 120.0f;
        // float b = 0.25f;
        // innerJoints[i]->SetStiffness(innerJoints[i]->GetStiffness()/200.0f);
    }
}


// Function to create a circle and return its body
b2Body* createCircle(b2World& world, float x, float y, float radius, bool dynamic = true) {
    b2BodyDef bodyDef;
    bodyDef.type = dynamic ? b2_dynamicBody : b2_staticBody;
    bodyDef.position.Set(x, y);
    b2Body* body = world.CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;
    fixtureDef.friction = 0.3f;
    body->CreateFixture(&fixtureDef);
    return body;
}

b2DistanceJoint *createDistanceJoint(b2World& world, b2Body* bodyA, b2Body* bodyB, float f, float b) {
    b2DistanceJointDef jb;

    // jb.Initialize(bodyA, bodyB, bodyA->GetPosition(), bodyB->GetPosition());
    // jb.collideConnected = false;
    // jb.length = (bodyA->GetPosition() - bodyB->GetPosition()).Length() * slack;

    jb.bodyA = bodyA;
    jb.bodyB = bodyB;
    jb.localAnchorA = bodyA->GetLocalCenter();
    jb.localAnchorB = bodyB->GetLocalCenter();
    b2Vec2 p1 = bodyA->GetWorldPoint(jb.localAnchorA);
    b2Vec2 p2 = bodyB->GetWorldPoint(jb.localAnchorB);
    b2Vec2 d = p2 - p1;
    jb.length = d.Length();
    b2LinearStiffness(jb.stiffness, jb.damping, f, b, jb.bodyA, jb.bodyB);
    

    // world.CreateJoint(&jb);

    return static_cast<b2DistanceJoint *>(world.CreateJoint(&jb));
}


SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
    SDL_Surface* surface = IMG_Load(filename);
    if (!surface) {
        std::cerr << "Error loading texture: " << IMG_GetError() << std::endl;
        exit(1);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "Error creating texture: " << SDL_GetError() << std::endl;
        exit(1);
    }

    return texture;
}

void renderTexture(SDL_Renderer* renderer, const std::vector<b2Body*>& particles, SDL_Texture* texture) {
    if (particles.empty()) return;

    // Determine the bounds of the circle
    float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
    for (const auto& body : particles) {
        b2Vec2 pos = body->GetPosition();
        minX = std::min(minX, pos.x);
        maxX = std::max(maxX, pos.x);
        minY = std::min(minY, pos.y);
        maxY = std::max(maxY, pos.y);
    }

    SDL_Rect dstRect;
    dstRect.w = static_cast<int>((maxX - minX + 2 * RADIUS) * SCALE/2);
    dstRect.h = static_cast<int>((maxY - minY + 2 * RADIUS) * SCALE/2);
    dstRect.x = static_cast<int>((minX - RADIUS) * SCALE) + dstRect.w/2;
    dstRect.y = static_cast<int>((minY - RADIUS) * SCALE) + dstRect.h/2;

    // Render the texture to cover the circle
    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);
}

void renderParticles(SDL_Renderer* renderer, b2World& world) {
        for (b2Body* body = world.GetBodyList(); body != nullptr; body = body->GetNext()) {
            if (body->GetType() == b2_dynamicBody) {
                b2Vec2 position = body->GetPosition();
                b2CircleShape* circle = static_cast<b2CircleShape*>(body->GetFixtureList()->GetShape());
                int pixelRadius = static_cast<int>(circle->m_radius * SCALE);
                int centerX = static_cast<int>(position.x * SCALE);
                int centerY = static_cast<int>(position.y * SCALE);
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
                filledCircleRGBA(renderer, centerX, centerY, pixelRadius, 255, 0, 0, 255); // Draw filled circle
            } else {
                // Render static objects in grey
                b2Vec2 position = body->GetPosition();
                b2PolygonShape* box = static_cast<b2PolygonShape*>(body->GetFixtureList()->GetShape());
                b2Vec2 vertices[4];
                for (int i = 0; i < 4; ++i) {
                    vertices[i] = body->GetWorldPoint(box->m_vertices[i]);
                }
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
                SDL_Rect rect;
                rect.x = (int)(vertices[0].x * SCALE);
                rect.y = (int)(vertices[0].y * SCALE);
                rect.w = (int)((vertices[2].x - vertices[0].x) * SCALE);
                rect.h = (int)((vertices[2].y - vertices[0].y) * SCALE);
                SDL_RenderFillRect(renderer, &rect);
            }
        }
}

void renderPolygon(SDL_Renderer* renderer, b2World& world) {
    // Render a polygon using filledPolygonRGBA whose vertices are defined by the particles from the particles vector
    // The polygon should be filled with blue
    // The outline of the polygon should be drawn in black
    std::vector<Sint16> vx;
    std::vector<Sint16> vy;
    for (auto body = particles.begin(); body != particles.end(); body = next(body)) {
        if ((*body)->GetType() == b2_dynamicBody) {
            b2Vec2 position = (*body)->GetPosition();
            b2CircleShape* circle = static_cast<b2CircleShape*>((*body)->GetFixtureList()->GetShape());
            int pixelRadius = static_cast<int>(circle->m_radius * SCALE);
            int centerX = static_cast<int>(position.x * SCALE);
            int centerY = static_cast<int>(position.y * SCALE);
            vx.push_back(centerX);
            vy.push_back(centerY);
        }
    }
    filledPolygonRGBA(renderer, vx.data(), vy.data(), vx.size(), 92, 127, 186, 255);

}

void renderCatEyes(SDL_Renderer* renderer, b2World& world) {
    // Render each eye as two thick lines in a roof shape
    // The eyes should be bound to particles at index NUM_PARTICLES/6 and 5*NUM_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/2 + M_PI/6;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/2 - M_PI/6;

    int xA = static_cast<int>(posA.x * SCALE);
    int yA = static_cast<int>(posA.y * SCALE);
    int xB = static_cast<int>(posB.x * SCALE);
    int yB = static_cast<int>(posB.y * SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * SCALE);
    int xBOpposite = static_cast<int>(posBOpposite.x * SCALE);
    int yBOpposite = static_cast<int>(posBOpposite.y * SCALE);

    int size = 8;

    b2Mat33 leftEye;
    leftEye.ex.Set(xA - size, yA + size, 1);
    leftEye.ey.Set(xA, yA, 1);
    leftEye.ez.Set(xA + size, yA + size, 1);


    b2Mat33 rightEye;
    rightEye.ex.Set(xB - size, yB + size, 1);
    rightEye.ey.Set(xB, yB, 1);
    rightEye.ez.Set(xB + size, yB + size, 1);

    // Move eye coordinates to inside body
    leftEye.ex = leftEye.ex + 0.2f * b2Vec3(xAOpposite - xA, yAOpposite - yA, 0);
    leftEye.ey = leftEye.ey + 0.2f * b2Vec3(xAOpposite - xA, yAOpposite - yA, 0);
    leftEye.ez = leftEye.ez + 0.2f * b2Vec3(xAOpposite - xA, yAOpposite - yA, 0);

    rightEye.ex = rightEye.ex + 0.2f * b2Vec3(xBOpposite - xB, yBOpposite - yB, 0);
    rightEye.ey = rightEye.ey + 0.2f * b2Vec3(xBOpposite - xB, yBOpposite - yB, 0);
    rightEye.ez = rightEye.ez + 0.2f * b2Vec3(xBOpposite - xB, yBOpposite - yB, 0);

    leftEye.ex = leftEye.ex - b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);
    leftEye.ey = leftEye.ey - b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);
    leftEye.ez = leftEye.ez - b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);

    rightEye.ex = rightEye.ex - b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);
    rightEye.ey = rightEye.ey - b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);
    rightEye.ez = rightEye.ez - b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);

    // Rotate eye coordinates
    b2Mat33 rotationMatrixA;
    rotationMatrixA.ex.Set(cos(angle), -sin(angle), 0);
    rotationMatrixA.ey.Set(sin(angle), cos(angle), 0);
    rotationMatrixA.ez.Set(0, 0, 1);
    
    b2Mat33 rotationMatrixB;
    rotationMatrixB.ex.Set(cos(angle2), -sin(angle2), 0);
    rotationMatrixB.ey.Set(sin(angle2), cos(angle2), 0);
    rotationMatrixB.ez.Set(0, 0, 1);


    // Multiply the eye by the rotation matrix
    leftEye = rotationMatrixA * leftEye;
    rightEye = rotationMatrixB * rightEye;

    leftEye.ex = leftEye.ex + b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);
    leftEye.ey = leftEye.ey + b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);
    leftEye.ez = leftEye.ez + b2Vec3(xA + 0.2f * (xAOpposite - xA), yA + 0.2f*(yAOpposite - yA), 0);

    rightEye.ex = rightEye.ex + b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);
    rightEye.ey = rightEye.ey + b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);
    rightEye.ez = rightEye.ez + b2Vec3(xB + 0.2f * (xBOpposite - xB), yB + 0.2f*(yBOpposite - yB), 0);


    // Move eyes down
    int verShift = 5;
    b2Vec3 shiftA = rotationMatrixA * b2Vec3(0, verShift, 0);
    b2Vec3 shiftB = rotationMatrixB * b2Vec3(0, verShift, 0);

    leftEye.ex = leftEye.ex + shiftA;
    leftEye.ey = leftEye.ey + shiftA;
    leftEye.ez = leftEye.ez + shiftA;

    rightEye.ex = rightEye.ex + shiftB;
    rightEye.ey = rightEye.ey + shiftB;
    rightEye.ez = rightEye.ez + shiftB;

    // Render eyes
    thickLineRGBA(renderer, rightEye.ex.x, rightEye.ex.y, rightEye.ey.x, rightEye.ey.y, 2, 50, 75, 132, 255);
    thickLineRGBA(renderer, rightEye.ey.x, rightEye.ey.y, rightEye.ez.x, rightEye.ez.y, 2, 50, 75, 132, 255);

    thickLineRGBA(renderer, leftEye.ex.x, leftEye.ex.y, leftEye.ey.x, leftEye.ey.y, 2, 50, 75, 132, 255);
    thickLineRGBA(renderer, leftEye.ey.x, leftEye.ey.y, leftEye.ez.x, leftEye.ez.y, 2, 50, 75, 132, 255);
    
}

void renderCatLegs(SDL_Renderer* renderer, b2World& world) {
    // Render cat legs as semi-circles using arcRGBA
    // The eyes should be bound to particles at index NUM_PARTICLES/6 and 5*NUM_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/2 + M_PI/6;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/2 - M_PI/6;

    int xA = static_cast<int>(posA.x * SCALE);
    int yA = static_cast<int>(posA.y * SCALE);
    int xB = static_cast<int>(posB.x * SCALE);
    int yB = static_cast<int>(posB.y * SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * SCALE);
    int xBOpposite = static_cast<int>(posBOpposite.x * SCALE);
    int yBOpposite = static_cast<int>(posBOpposite.y * SCALE);

    // Define centers of the semi-circles
    b2Vec3 centerA = b2Vec3(xA + 0.8 * (xAOpposite - xA), yA + 0.8 * (yAOpposite - yA), 1);
    b2Vec3 centerB = b2Vec3(xB + 0.8 * (xBOpposite - xB), yB + 0.8 * (yBOpposite - yB), 1);

    // Define the semi-circle for the left leg
    int radius = 7;
    int startAngle = angle * 180 / M_PI;
    int endAngle = (angle + M_PI) * 180 / M_PI;
    arcRGBA(renderer, centerB.x, centerB.y, radius, startAngle, endAngle, 50, 75, 132, 255);

    // Define the semi-circle for the right leg
    startAngle = angle2 * 180 / M_PI;
    endAngle = (angle2 + M_PI) * 180 / M_PI;
    arcRGBA(renderer, centerA.x, centerA.y, radius, startAngle, endAngle, 50, 75, 132, 255);


}

void renderCatMouth(SDL_Renderer* renderer, b2World& world) {
    // Render cat mouth as two arcs using arcRGBA
    // The mouth should be bound to particles at index NUM_PARTICLES/6 and 5*NUM_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[NUM_PARTICLES/4];

    b2Body* bodyAOpposite = particles[3*NUM_PARTICLES/4];

    b2Vec2 posA = bodyA->GetPosition();

    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();

    // Get angle of the line between bodyA and boddyAOpposite
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) + M_PI/2;

    int xA = static_cast<int>(posA.x * SCALE);
    int yA = static_cast<int>(posA.y * SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * SCALE);

    b2Mat33 rotationMatrixA;
    rotationMatrixA.ex.Set(cos(angle), -sin(angle), 0);
    rotationMatrixA.ey.Set(sin(angle), cos(angle), 0);
    rotationMatrixA.ez.Set(0, 0, 1);

    int radius = 10;
    b2Vec3 shiftA = rotationMatrixA * b2Vec3(radius, 0, 0);

    // Define centers of the arcs
    b2Vec3 centerA = b2Vec3(xA + 0.5 * (xAOpposite - xA), yA + 0.5 * (yAOpposite - yA), 1) - shiftA;
    b2Vec3 centerB = b2Vec3(xA + 0.5 * (xAOpposite - xA), yA + 0.5 * (yAOpposite - yA), 1) + shiftA;

    // Define the arc for the left side of the mouth
    int startAngle = (angle) * 180 / M_PI;
    int endAngle = (angle + M_PI - M_PI/4) * 180 / M_PI;
    arcRGBA(renderer, centerA.x, centerA.y, radius, startAngle, endAngle, 50, 75, 132, 255);
    arcRGBA(renderer, centerA.x, centerA.y, radius+1, startAngle, endAngle, 50, 75, 132, 255);

    // Define the arc for the right side of the mouth
    startAngle = (angle + M_PI/4) * 180 / M_PI;
    endAngle = (angle + M_PI) * 180 / M_PI;
    arcRGBA(renderer, centerB.x, centerB.y, radius, startAngle, endAngle, 50, 75, 132, 255);
    arcRGBA(renderer, centerB.x, centerB.y, radius+1, startAngle, endAngle, 50, 75, 132, 255);

    // Render nouse as black filled circle
    filledCircleRGBA(renderer, xA + 0.5 * (xAOpposite - xA), yA + 0.5 * (yAOpposite - yA), 2, 50, 75, 132, 255);


}

void renderCatEars(SDL_Renderer* renderer, b2World& world) {
    // Render cat ears as triangles using filledTrigonRGBA
    // Triangles should be bound to particles at index NUM_PARTICLES/6 and 5*NUM_PARTICLES/6
    // The triangles should be filled with pink
    // The outline of the triangles should be drawn in black

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/6 + M_PI/3;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/6 + M_PI;

    int xA = static_cast<int>(posA.x * SCALE);
    int yA = static_cast<int>(posA.y * SCALE);
    int xB = static_cast<int>(posB.x * SCALE);
    int yB = static_cast<int>(posB.y * SCALE);

    b2Mat33 triangleA;
    triangleA.ex.Set(xA - 10, yA + 10, 1);
    triangleA.ey.Set(xA + 10, yA + 10, 1);
    triangleA.ez.Set(xA, yA - 10, 1);

    b2Mat33 triangleB;
    triangleB.ex.Set(xB - 10, yB + 10, 1);
    triangleB.ey.Set(xB + 10, yB + 10, 1);
    triangleB.ez.Set(xB, yB - 10, 1);

    // Transform triangle coordinates
    triangleA.ex = triangleA.ex - b2Vec3(xA, yA, 0);
    triangleA.ey = triangleA.ey - b2Vec3(xA, yA, 0);
    triangleA.ez = triangleA.ez - b2Vec3(xA, yA, 0);

    triangleB.ex = triangleB.ex - b2Vec3(xB, yB, 0);
    triangleB.ey = triangleB.ey - b2Vec3(xB, yB, 0);
    triangleB.ez = triangleB.ez - b2Vec3(xB, yB, 0);

    b2Mat33 rotationMatrixA;
    rotationMatrixA.ex.Set(cos(angle), -sin(angle), 0);
    rotationMatrixA.ey.Set(sin(angle), cos(angle), 0);
    rotationMatrixA.ez.Set(0, 0, 1);

    b2Mat33 rotationMatrixB;
    rotationMatrixB.ex.Set(cos(angle2), -sin(angle2), 0);
    rotationMatrixB.ey.Set(sin(angle2), cos(angle2), 0);
    rotationMatrixB.ez.Set(0, 0, 1);

    // Multiply the triangle by the rotation matrix
    triangleA = rotationMatrixA * triangleA;
    triangleB = rotationMatrixB * triangleB;

    // Translate the triangle back to the original position
    triangleA.ex = triangleA.ex + b2Vec3(xA, yA, 0);
    triangleA.ey = triangleA.ey + b2Vec3(xA, yA, 0);
    triangleA.ez = triangleA.ez + b2Vec3(xA, yA, 0);

    triangleB.ex = triangleB.ex + b2Vec3(xB, yB, 0);
    triangleB.ey = triangleB.ey + b2Vec3(xB, yB, 0);
    triangleB.ez = triangleB.ez + b2Vec3(xB, yB, 0);

    filledTrigonRGBA(renderer, triangleA.ex.x, triangleA.ex.y, triangleA.ey.x, triangleA.ey.y, triangleA.ez.x, triangleA.ez.y, 92, 127, 186, 255);
    filledTrigonRGBA(renderer, triangleB.ex.x, triangleB.ex.y, triangleB.ey.x, triangleB.ey.y, triangleB.ez.x, triangleB.ez.y, 92, 127, 186, 255);

    // filledTrigonRGBA(renderer, xA-10, yA+10, xA + 10, yA + 10, xA, yA - 10, 255, 105, 180, 255);
    // filledTrigonRGBA(renderer, xB-10, yB+10, xB + 10, yB + 10, xB , yB - 10, 255, 105, 180, 255);


}

void renderStaticObjects(SDL_Renderer* renderer, b2World& world) {
    for (auto body = staticObjects.begin(); body != staticObjects.end(); body = next(body)) {
        if ((*body)->GetType() == b2_staticBody) {
            b2PolygonShape* shape = static_cast<b2PolygonShape*>((*body)->GetFixtureList()->GetShape());
            SDL_Rect rect = box2DToSDL((*body)->GetPosition(), shape->m_vertices[2].x * 2, shape->m_vertices[2].y * 2);
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, SDL_ALPHA_OPAQUE);
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

void renderSushi(SDL_Renderer* renderer, b2World& world, b2Body& body) {
    // Render a sushi as a simple black rectangle

    b2Vec2 position = body.GetPosition();
    b2PolygonShape* box = static_cast<b2PolygonShape*>(body.GetFixtureList()->GetShape());
    b2Vec2 vertices[4];
    for (int i = 0; i < 4; ++i) {
        vertices[i] = body.GetWorldPoint(box->m_vertices[i]);
    }
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_Rect rect;
    rect.x = (int)(vertices[0].x * SCALE);
    rect.y = (int)(vertices[0].y * SCALE);
    rect.w = (int)((vertices[2].x - vertices[0].x) * SCALE);
    rect.h = (int)((vertices[2].y - vertices[0].y) * SCALE);
    SDL_RenderFillRect(renderer, &rect);
}

void renderSushis(SDL_Renderer* renderer, b2World& world) {
    for (auto& body : sushis) {
        renderSushi(renderer, world, *body);
    }
}

void renderObstacle(SDL_Renderer* renderer, b2World& world, b2Body& body) {
    // Render an obstacle as a polygon to allow rotation
    b2Fixture* fixture = body.GetFixtureList();
    while (fixture) {
        b2Shape* shape = fixture->GetShape();
        if (shape->GetType() == b2Shape::e_polygon) {
            b2PolygonShape* polygon = static_cast<b2PolygonShape*>(shape);
            const int32 vertexCount = polygon->m_count;
            Sint16 vx[vertexCount];
            Sint16 vy[vertexCount];

            for (int i = 0; i < vertexCount; ++i) {
                b2Vec2 wp = body.GetWorldPoint(polygon->m_vertices[i]);
                vx[i] = static_cast<Sint16>(wp.x * SCALE);
                vy[i] = static_cast<Sint16>(wp.y * SCALE);
            }

            // Draw filled polygon with rotation handled
            filledPolygonRGBA(renderer, vx, vy, vertexCount, 0, 0, 0, 255);
        
        }
        fixture = fixture->GetNext();
    }
}


void renderObstacles(SDL_Renderer* renderer, b2World& world) {
    for (auto& body : obstacles) {
        renderObstacle(renderer, world, *body);
    }
}

void generateRandomSushi(b2World& world) {

    float x = (rand() % (WINDOW_WIDTH - 100)) / SCALE;
    float y = (rand() % (WINDOW_HEIGHT - 100)) / SCALE;
    b2BodyDef rectangleBodyDef;
    rectangleBodyDef.type = b2_kinematicBody;
    rectangleBodyDef.position.Set(x, y);
    b2Body* rectangleBody = world.CreateBody(&rectangleBodyDef);
    b2PolygonShape rectangleBox;
    rectangleBox.SetAsBox(SUSHI_WIDTH, SUSHI_HEIGHT);
    b2FixtureDef rectangleFixtureDef;
    rectangleFixtureDef.shape = &rectangleBox;
    rectangleFixtureDef.density = 1.0f;
    rectangleFixtureDef.friction = 0.3f;
    rectangleBody->CreateFixture(&rectangleFixtureDef);
    sushis.push_back(rectangleBody);
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);  // Initialize SDL_image

    SDL_Window* window = SDL_CreateWindow("Box2D and SDL2 Example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WINDOW_WIDTH, WINDOW_HEIGHT, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);


    b2Vec2 gravity(0.0f, 10.0f); // Gravity vector
    b2World world(gravity);
    worldPtr = &world;

    // // Define the ground body.
    // b2BodyDef groundBodyDef;
    // groundBodyDef.position.Set(WINDOW_WIDTH / SCALE / 2, (WINDOW_HEIGHT / SCALE) - 0.5f);
    // b2Body* groundBody = world.CreateBody(&groundBodyDef);
    // b2PolygonShape groundBox;
    // groundBox.SetAsBox(WINDOW_WIDTH / SCALE / 2, 0.5f); // As wide as the screen
    // groundBody->CreateFixture(&groundBox, 0.0f);

    // Define the left wall body.
    b2BodyDef leftWallBodyDef;
    leftWallBodyDef.position.Set(5.0f, WINDOW_HEIGHT / SCALE / 2);
    b2Body* leftWallBody = world.CreateBody(&leftWallBodyDef);
    b2PolygonShape leftWallBox;
    leftWallBox.SetAsBox(0.5f, WINDOW_HEIGHT / SCALE / 4); // As tall as the screen
    leftWallBody->CreateFixture(&leftWallBox, 0.0f);
    staticObjects.push_back(leftWallBody);

    // Define the right wall body.
    b2BodyDef rightWallBodyDef;
    rightWallBodyDef.position.Set((WINDOW_WIDTH / SCALE) - 5.0f, WINDOW_HEIGHT / SCALE / 2);
    b2Body* rightWallBody = world.CreateBody(&rightWallBodyDef);
    b2PolygonShape rightWallBox;
    rightWallBox.SetAsBox(0.5f, WINDOW_HEIGHT / SCALE / 4); // As tall as the screen
    rightWallBody->CreateFixture(&rightWallBox, 0.0f);
    staticObjects.push_back(rightWallBody);

    // Make a horizontal rectangle obstacle
    // Kinematic obstacle
    b2BodyDef obstacleBodyDef;
    obstacleBodyDef.type = b2_kinematicBody;
    obstacleBodyDef.position.Set(10.0f, 10.0f);
    obstacleBodyDef.fixedRotation = false;
    obstacleBodyDef.gravityScale = 0;
    obstacleBodyDef.angularVelocity = 5.0f;
    b2Body* obstacleBody = world.CreateBody(&obstacleBodyDef);
    b2PolygonShape obstacleBox;
    obstacleBox.SetAsBox(4.0f, 0.5f);
    b2FixtureDef obstacleFixtureDef;
    obstacleFixtureDef.shape = &obstacleBox;
    obstacleFixtureDef.density = 1.0f;
    obstacleFixtureDef.friction = 0.3f;
    obstacleBody->CreateFixture(&obstacleFixtureDef);
    obstacles.push_back(obstacleBody);

    // Dynamic obstacle
    b2BodyDef obstacleBodyDef2;
    obstacleBodyDef2.type = b2_dynamicBody;
    obstacleBodyDef2.position.Set(20.0f, 10.0f);
    obstacleBodyDef2.fixedRotation = false;
    obstacleBodyDef2.gravityScale = 0;  // This ensures the body doesn't fall due to gravity
    b2Body* obstacleBody2 = world.CreateBody(&obstacleBodyDef2);

    b2PolygonShape obstacleBox2;
    obstacleBox2.SetAsBox(3.0f, 0.5f);  // Corrected variable name here
    b2FixtureDef obstacleFixtureDef2;
    obstacleFixtureDef2.shape = &obstacleBox2;
    obstacleFixtureDef2.density = 1.0f;
    obstacleFixtureDef2.friction = 0.3f;
    obstacleBody2->CreateFixture(&obstacleFixtureDef2);

    b2BodyDef anchorBodyDef;
    anchorBodyDef.type = b2_staticBody;
    anchorBodyDef.position.Set(20.0f, 10.0f);  // Same as the obstacle position
    b2Body* anchorBody = world.CreateBody(&anchorBodyDef);

    b2RevoluteJointDef jointDef;
    jointDef.Initialize(anchorBody, obstacleBody2, anchorBody->GetWorldCenter());
    jointDef.enableMotor = false;
    jointDef.enableLimit = false;
    b2RevoluteJoint* joint = (b2RevoluteJoint*)world.CreateJoint(&jointDef);

    obstacles.push_back(obstacleBody2);







    for (int i = 0; i < 15; i++) 
        generateRandomSushi(world);


    bool quit = false;
    SDL_Event event;

    float dt = 1.0f / 6000.0f; // Time step for simulation
    SDL_Texture* texture = loadTexture(renderer, "cat.bmp");
    std::cout << "Texture ID: " << texture << std::endl;

    while (!quit) {
        ////////////////////////// Mouse events //////////////////////////
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                quit = true;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                RADIUS = 1.0f;
                float centerX = event.button.x / SCALE;
                float centerY = event.button.y / SCALE;

                // Clear previous particles
                for (auto* p : particles) {
                    world.DestroyBody(p);
                }
                particles.clear();
                innerJoints.clear();
                outerJoints.clear();

                // Create particles in a circle
                for (int i = 0; i < NUM_PARTICLES; ++i) {
                    float angle = i * 2 * M_PI / NUM_PARTICLES;
                    float x = centerX + RADIUS * cos(angle);
                    float y = centerY + RADIUS * sin(angle);
                    b2Body* particle = createCircle(world, x, y, 0.05f);
                    particles.push_back(particle);
                }

                // Connect particles with distance joints to immediate neighbors
                b2DistanceJoint *jb;
                for (int i = 0; i < NUM_PARTICLES; ++i) {
                    b2Body* bodyA = particles[i];
                    b2Body* bodyB = particles[(i + 1) % NUM_PARTICLES];
                    jb = createDistanceJoint(world, bodyA, bodyB, 120.0f, 0);
                    outerJoints.push_back(jb);
                    if (i % (NUM_PARTICLES/20) == 0) {
                        b2Body* bodyC = particles[(i + NUM_PARTICLES/2) % NUM_PARTICLES];
                        jb = createDistanceJoint(world, bodyA, bodyC, 15.0f, 0.25f);
                        innerJoints.push_back(jb);
                    }
                }

            }
        }

        ////////////////////////// Check for collisions //////////////////////////
        // Check if any of the particles touched any rectangles in sushis
        // std::cout << "Checking for collisions" << std::endl;
        std::vector<b2Body*> bodiesToRemove;
        for (auto* particle : particles) {
            for (auto* sushi : sushis) {
                if (b2TestOverlap(particle->GetFixtureList()->GetShape(), 0, sushi->GetFixtureList()->GetShape(), 0, particle->GetTransform(), sushi->GetTransform())) {
                    std::cout << "Particle collided with sushi!" << std::endl;
                    bodiesToRemove.push_back(sushi);
                }
            }
        }
        // std::cout << "Collisions: " << bodiesToRemove.size() << std::endl;

        // Remove the sushi bodies that were touched
        for (b2Body* body : bodiesToRemove) {
            world.DestroyBody(body);
            sushis.remove(body);

            // Make circle bigger by lengthening the distance joints and increasing radius
            RADIUS = RADIUS * 1.1f;
            changeParticleCircleRadius();
        }
        bodiesToRemove.clear();
        // std::cout << "over" << std::endl;


        ////////////////////////// Forward time //////////////////////////
        // Set dt to ensure stable 60 fps
        static std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed_seconds = now - last;
        last = now;
        dt = elapsed_seconds.count();

        world.Step(dt, 6, 2); // Step the world forward
        // std::cout << "Step" << std::endl;

        ////////////////////////// Rendering //////////////////////////

        // SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_SetRenderDrawColor(renderer, 100, 100, 100, SDL_ALPHA_OPAQUE);

        SDL_RenderClear(renderer);
        // Render sushi
        if (!sushis.empty()) 
            renderSushis(renderer, world);
        // renderTexture(renderer, particles, texture);
        // renderParticles(renderer, world);
        renderPolygon(renderer, world);

        // Render cat body parts
        renderCatEars(renderer, world);
        renderCatEyes(renderer, world);
        renderCatLegs(renderer, world);
        renderCatMouth(renderer, world);

        // Render static objects in grey
        renderStaticObjects(renderer, world);
        renderObstacles(renderer, world);


        // Present the rendered scene
        SDL_RenderPresent(renderer);
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
