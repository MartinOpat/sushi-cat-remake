#ifndef GLOBALS_H
#define GLOBALS_H

#include "../box2d-main/include/box2d/box2d.h"

#include <SDL2/SDL.h>


extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
extern float SCALE;

extern int NUM_CAT_PARTICLES;
extern int BASE_CAT_RADIUS;
extern int CAT_SPAWN_HEIGHT;

extern int SUSHI_WIDTH;
extern int SUSHI_HEIGHT;

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

// Helper function
inline SDL_Rect box2DToSDL(const b2Vec2& position, float width, float height) {
    SDL_Rect rect;
    rect.x = (int)(position.x * SCALE - width * SCALE / 2);
    rect.y = (int)(position.y * SCALE - height * SCALE / 2);
    rect.w = (int)(width * SCALE);
    rect.h = (int)(height * SCALE);
    return rect;
}

#endif // GLOBALS_H