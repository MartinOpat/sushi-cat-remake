#include "object.h"

Object::Object(b2Vec2 position, b2BodyType type, bool fixedRotation, float angVel, float gravityScale) {
    bodyDef.type = type;
    bodyDef.position = position;
    bodyDef.fixedRotation = fixedRotation;
    bodyDef.angularVelocity = angVel;
    bodyDef.gravityScale = gravityScale;
}