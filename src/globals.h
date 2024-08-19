#ifndef GLOBALS_H
#define GLOBALS_H

#include <box2d/box2d.h>

#include <SDL2/SDL.h>
// #include "../SDL2/x86_64-w64-mingw32/include/SDL2/SDL.h"
#include <SDL2/SDL_image.h>
#include <iostream>


extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;

extern float PHYSICS_SCALE;
extern float WINDOW_SCALE;

extern int NUM_CAT_PARTICLES;
extern int BASE_CAT_RADIUS;
extern int CAT_SPAWN_HEIGHT;

extern int SUSHI_WIDTH;
extern int SUSHI_HEIGHT;

extern int BOX_OFFSET;

// Define multiplaction between a b2Mat33 and a b2Vec3 via *
inline b2Vec3 operator*(const b2Mat33& A, const b2Vec3& v) {
    return b2Vec3(b2Dot(A.ex, v), b2Dot(A.ey, v), b2Dot(A.ez, v));
}

// Define matrix multiplication via * for b2Mat33
inline b2Mat33 operator*(const b2Mat33& A, const b2Mat33& B) {
    b2Mat33 result;
    result.ex = A * B.ex;
    result.ey = A * B.ey;
    result.ez = A * B.ez;
    return result;
}

// Define / operator for b2Vec3 and float
inline b2Vec3 operator/(const b2Vec3& v, float a) {
    return b2Vec3(v.x / a, v.y / a, v.z / a);
}

// Define / operator for b2Vec2 and float
inline b2Vec2 operator/(const b2Vec2& v, float a) {
    return b2Vec2(v.x / a, v.y / a);
}



// Helper function
inline SDL_Rect box2DToSDL(const b2Vec2& position, float width, float height) {
    SDL_Rect rect;
    rect.x = (int)(position.x * PHYSICS_SCALE - width * PHYSICS_SCALE / 2);
    rect.y = (int)(position.y * PHYSICS_SCALE - height * PHYSICS_SCALE / 2);
    rect.w = (int)(width * PHYSICS_SCALE);
    rect.h = (int)(height * PHYSICS_SCALE);
    return rect;
}

inline SDL_Texture* loadTexture(SDL_Renderer* renderer, const char* filename) {
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

#endif // GLOBALS_H