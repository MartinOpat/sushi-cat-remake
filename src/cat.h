#ifndef CAT_H
#define CAT_H

#include "globals.h"

#include <SDL2/SDL2_gfxPrimitives.h> 
#include <SDL2/SDL_image.h>
#include <vector>

// Define your class and its members
class Cat {
public:
    Cat(float x, float y, b2World& world);

    void render(SDL_Renderer* renderer);
    void updateRadius(float newRadius);

    float getRadius() {return radius;};

private:
    void Cat::renderCatParticles(SDL_Renderer* renderer);
    void renderPolygon(SDL_Renderer* renderer);
    void renderCatEyes(SDL_Renderer* renderer);
    void renderCatLegs(SDL_Renderer* renderer);
    void renderCatMouth(SDL_Renderer* renderer);
    void renderCatEars(SDL_Renderer* renderer);

    std::vector<b2DistanceJoint *> outerJoints;
    std::vector<b2DistanceJoint *> innerJoints;
    std::vector<b2Body*> particles;

    float radius;
    b2Body* body;

};

#endif // CAT_H