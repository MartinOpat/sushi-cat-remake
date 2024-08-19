#ifndef CAT_H
#define CAT_H

#include "globals.h"

#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <vector>
#include <iostream>
#include <chrono>

// Define your class and its members
class Cat {
public:
    Cat(float x, float y, b2World *world);
    void destroy(b2World *world);

    void render(SDL_Renderer* renderer);
    void updateRadius(float newRadius);

    float getRadius() {return radius;};
    std::vector<b2Body*>& getParticles() {return particles;};
    int getEatenSushis() {return eatenSushis;};

    void setPosition(float x, float y);
    void eatSushi();

    bool isOffscreen() {return particles[0]->GetPosition().y > WINDOW_HEIGHT/SCALE;};
    bool isStuck(bool &isDragging);
    void toggleSquish(b2World *world);

    bool wasStuck = false;
private:
    // ----------------- Rendering functions -----------------
    void renderCatParticles(SDL_Renderer* renderer);
    void renderPolygon(SDL_Renderer* renderer);
    void renderCatEyes(SDL_Renderer* renderer);
    void renderCatLegs(SDL_Renderer* renderer);
    void renderCatMouth(SDL_Renderer* renderer);
    void renderCatEars(SDL_Renderer* renderer);

    // ----------------- Cat init. functions -----------------
    b2Body* createCircle(b2World& world, float x, float y, float radius);
    b2DistanceJoint *createDistanceJoint(b2World& world, b2Body* bodyA, b2Body* bodyB, float f, float b);

    // ----------------- Cat physics attributes -----------------
    std::vector<b2DistanceJoint *> outerJoints;
    std::vector<b2DistanceJoint *> innerJoints;
    std::vector<b2Body*> particles;

    float innerJointsFreq = 15.0f;
    float innerJointsBaseFreq = 15.0f;
    float innerJointsSquishFreq = 1.0f;

    float innerJointsDamping = 0.25f;
    float innerJointsBaseDamping = 0.25f;
    float innerJointsSquishDamping = 0.1f;


    // ----------------- Cat rendering attributes -----------------
    float radius;
    int eatenSushis = 0;
    float oldAngle;
};

#endif // CAT_H