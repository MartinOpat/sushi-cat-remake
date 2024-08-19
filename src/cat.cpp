#include "cat.h"

// -------------------- Constructors --------------------

Cat::Cat(float x, float y, b2World *world) {
    radius = BASE_CAT_RADIUS;

    // Clear previous particles
    for (auto* p : particles) {
        world->DestroyBody(p);
    }
    particles.clear();
    innerJoints.clear();
    outerJoints.clear();

    // Create particles in a circle
    for (int i = 0; i < NUM_CAT_PARTICLES; ++i) {
        float angle = i * 2 * M_PI / NUM_CAT_PARTICLES;
        float centerX = x + radius * cos(angle);
        float centerY = y + radius * sin(angle);
        b2Body* particle = createCircle(*world, centerX, centerY, 0.05f);
        particles.push_back(particle);
    }

    // Connect particles with distance joints to immediate neighbors
    b2DistanceJoint *jb;
    for (int i = 0; i < NUM_CAT_PARTICLES; ++i) {
        b2Body* bodyA = particles[i];
        b2Body* bodyB = particles[(i + 1) % NUM_CAT_PARTICLES];
        jb = createDistanceJoint(*world, bodyA, bodyB, 120.0f, 0);  // default: f=120.0f, b=0
        outerJoints.push_back(jb);
        if (i % (NUM_CAT_PARTICLES/20) == 0) {
            b2Body* bodyC = particles[(i + NUM_CAT_PARTICLES/2) % NUM_CAT_PARTICLES];
            jb = createDistanceJoint(*world, bodyA, bodyC, innerJointsFreq, innerJointsDamping);  // default: f=15.0f, b=0.25f
            innerJoints.push_back(jb);
        }
    }

    b2Body* bodyA = particles[NUM_CAT_PARTICLES/2];
    b2Body* bodyB = particles[0];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();
    oldAngle = atan2(posB.y - posA.y, posB.x - posA.x);
}

void Cat::destroy(b2World* world) {
    for (auto* p : particles) {
        world->DestroyBody(p);
    }
}

// Function to create a circle and return its body
b2Body* Cat::createCircle(b2World& world, float x, float y, float radius) {
    b2BodyDef bodyDef;
    bodyDef.type = b2_dynamicBody;
    bodyDef.position.Set(x, y);
    b2Body* body = world.CreateBody(&bodyDef);

    b2CircleShape circle;
    circle.m_radius = radius;

    b2FixtureDef fixtureDef;
    fixtureDef.shape = &circle;
    fixtureDef.density = 1.0f;  // default 1.0f
    fixtureDef.friction = 0.3f;  // default 0.3f
    body->CreateFixture(&fixtureDef);
    return body;
}

b2DistanceJoint* Cat::createDistanceJoint(b2World& world, b2Body* bodyA, b2Body* bodyB, float f, float b) {
    b2DistanceJointDef jb;

    jb.bodyA = bodyA;
    jb.bodyB = bodyB;
    jb.localAnchorA = bodyA->GetLocalCenter();
    jb.localAnchorB = bodyB->GetLocalCenter();
    b2Vec2 p1 = bodyA->GetWorldPoint(jb.localAnchorA);
    b2Vec2 p2 = bodyB->GetWorldPoint(jb.localAnchorB);
    b2Vec2 d = p2 - p1;
    jb.length = d.Length();
    b2LinearStiffness(jb.stiffness, jb.damping, f, b, jb.bodyA, jb.bodyB);
    
    return static_cast<b2DistanceJoint *>(world.CreateJoint(&jb));
}

// -------------------- Updates --------------------

void Cat::updateRadius(float newRadius) {
    radius = newRadius;

    // Get original rotation, i.e. the angle 
    // At default rotation 4*NUM_CAT_PARTICLES/6 is at the top, 1*NUM_CAT_PARTICLES/6 is at the bottom
    b2Body* bodyA = particles[NUM_CAT_PARTICLES/2];
    b2Body* bodyB = particles[0];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();
    oldAngle = atan2(posB.y - posA.y, posB.x - posA.x);



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
        float x = centerX + radius * cos(angle + oldAngle);
        float y = centerY + radius * sin(angle + oldAngle);
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
        // innerJoints[i]->SetStiffness(innerJoints[i]->GetStiffness()/20.0f);
    }
}

void Cat::setPosition(float x, float y) {
    // Calculate the center of the circle
    float centerX = 0.0f, centerY = 0.0f;
    for (const auto& body : particles) {
        centerX += body->GetPosition().x;
        centerY += body->GetPosition().y;
    }
    centerX /= particles.size();
    centerY /= particles.size();

    // Update each particle's position to the new center
    for (size_t i = 0; i < particles.size(); ++i) {
        float xNew = x + (particles[i]->GetPosition().x - centerX);
        float yNew = y + (particles[i]->GetPosition().y - centerY);
        particles[i]->SetTransform(b2Vec2(xNew, yNew), particles[i]->GetAngle());
        particles[i]->SetLinearVelocity(b2Vec2(0.0f, 0.001f));
    }
}

void Cat::eatSushi() {
    eatenSushis++;
    updateRadius(BASE_CAT_RADIUS + eatenSushis * BASE_CAT_RADIUS / 20.0f);
}

bool Cat::isStuck(bool &isDragging) {
    if (isDragging) {
        return false;
    }
    // Check if the cat is stuck by checking the average velocity of particles
    float avgVelocity = 0.0f;
    for (const auto& body : particles) {
        avgVelocity += body->GetLinearVelocity().Length();
    }
    avgVelocity /= particles.size();
    // std::cout << "Avg. velocity: " << avgVelocity << std::endl;


    // Check that avg. velocity is below a threshold for 3 consecutive seconds
    static std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed_seconds = now - last;

    // std::cout << "Elapsed time: " << elapsed_seconds.count() << std::endl;

    if (avgVelocity < 0.01f) {
        if (elapsed_seconds.count() > 3.0f) {
            return true;
        }
    } else {
        last = now;
    }
    return false;
}

void Cat::toggleSquish(b2World *world) {
    // Toggle the squish state of the cat
    if (innerJointsFreq == innerJointsBaseFreq) {
        innerJointsFreq = innerJointsSquishFreq;
        innerJointsDamping = innerJointsSquishDamping;
    } else {
        innerJointsFreq = innerJointsBaseFreq;
        innerJointsDamping = innerJointsBaseDamping;
    }

    // Free up old joints and clear the array
    for (auto* joint : innerJoints) {
        world->DestroyJoint(joint);
    }
    innerJoints.clear();

    // Update inner joints
    b2DistanceJoint *jb;
    for (int i = 0; i < NUM_CAT_PARTICLES; ++i) {
        b2Body* bodyA = particles[i];
        b2Body* bodyB = particles[(i + 1) % NUM_CAT_PARTICLES];
        jb = createDistanceJoint(*world, bodyA, bodyB, 120.0f, 0);  // default: f=120.0f, b=0
        outerJoints.push_back(jb);
        if (i % (NUM_CAT_PARTICLES/20) == 0) {
            b2Body* bodyC = particles[(i + NUM_CAT_PARTICLES/2) % NUM_CAT_PARTICLES];
            jb = createDistanceJoint(*world, bodyA, bodyC, innerJointsFreq, innerJointsDamping);  // default: f=15.0f, b=0.25f
            innerJoints.push_back(jb);
        }
    }
}

// -------------------- Rendering --------------------

void Cat::render(SDL_Renderer* renderer) {
    // renderCatParticles(renderer);
    renderPolygon(renderer);
    renderCatEyes(renderer);
    renderCatLegs(renderer);
    renderCatMouth(renderer);
    renderCatEars(renderer);
}


void Cat::renderCatParticles(SDL_Renderer* renderer) {
    for (auto body = particles.begin(); body != particles.end(); body = next(body)) {
        b2Vec2 position = (*body)->GetPosition();
        b2CircleShape* circle = static_cast<b2CircleShape*>((*body)->GetFixtureList()->GetShape());
        int pixelRadius = static_cast<int>(circle->m_radius * PHYSICS_SCALE);
        int centerX = static_cast<int>(position.x * PHYSICS_SCALE);
        int centerY = static_cast<int>(position.y * PHYSICS_SCALE);
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
        filledCircleRGBA(renderer, centerX, centerY, pixelRadius, 255, 0, 0, 255); // Draw filled circle
    }
}

void Cat::renderPolygon(SDL_Renderer* renderer) {
    // Render a polygon using filledPolygonRGBA whose vertices are defined by the particles from the particles vector
    // The polygon should be filled with blue
    // The outline of the polygon should be drawn in black
    std::vector<Sint16> vx;
    std::vector<Sint16> vy;
    for (auto body = particles.begin(); body != particles.end(); body = next(body)) {
        if ((*body)->GetType() == b2_dynamicBody) {
            b2Vec2 position = (*body)->GetPosition();
            int centerX = static_cast<int>(position.x * PHYSICS_SCALE);
            int centerY = static_cast<int>(position.y * PHYSICS_SCALE);
            vx.push_back(centerX);
            vy.push_back(centerY);
        }
    }
    filledPolygonRGBA(renderer, vx.data(), vy.data(), vx.size(), 92, 127, 186, 255);

}

void Cat::renderCatEyes(SDL_Renderer* renderer) {
    // Render each eye as two thick lines in a roof shape
    // The eyes should be bound to particles at index NUM_CAT_PARTICLES/6 and 5*NUM_CAT_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_CAT_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_CAT_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_CAT_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_CAT_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/2 + M_PI/6;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/2 - M_PI/6;

    int xA = static_cast<int>(posA.x * PHYSICS_SCALE);
    int yA = static_cast<int>(posA.y * PHYSICS_SCALE);
    int xB = static_cast<int>(posB.x * PHYSICS_SCALE);
    int yB = static_cast<int>(posB.y * PHYSICS_SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * PHYSICS_SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * PHYSICS_SCALE);
    int xBOpposite = static_cast<int>(posBOpposite.x * PHYSICS_SCALE);
    int yBOpposite = static_cast<int>(posBOpposite.y * PHYSICS_SCALE);

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

void Cat::renderCatLegs(SDL_Renderer* renderer) {
    // Render cat legs as semi-circles using arcRGBA
    // The eyes should be bound to particles at index NUM_CAT_PARTICLES/6 and 5*NUM_CAT_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_CAT_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_CAT_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_CAT_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_CAT_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/2 + M_PI/6;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/2 - M_PI/6;

    int xA = static_cast<int>(posA.x * PHYSICS_SCALE);
    int yA = static_cast<int>(posA.y * PHYSICS_SCALE);
    int xB = static_cast<int>(posB.x * PHYSICS_SCALE);
    int yB = static_cast<int>(posB.y * PHYSICS_SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * PHYSICS_SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * PHYSICS_SCALE);
    int xBOpposite = static_cast<int>(posBOpposite.x * PHYSICS_SCALE);
    int yBOpposite = static_cast<int>(posBOpposite.y * PHYSICS_SCALE);

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

void Cat::renderCatMouth(SDL_Renderer* renderer) {
    // Render cat mouth as two arcs using arcRGBA
    // The mouth should be bound to particles at index NUM_CAT_PARTICLES/6 and 5*NUM_CAT_PARTICLES/6

    if (particles.empty()) return;
    b2Body* bodyA = particles[NUM_CAT_PARTICLES/4];

    b2Body* bodyAOpposite = particles[3*NUM_CAT_PARTICLES/4];

    b2Vec2 posA = bodyA->GetPosition();

    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();

    // Get angle of the line between bodyA and boddyAOpposite
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) + M_PI/2;

    int xA = static_cast<int>(posA.x * PHYSICS_SCALE);
    int yA = static_cast<int>(posA.y * PHYSICS_SCALE);

    int xAOpposite = static_cast<int>(posAOpposite.x * PHYSICS_SCALE);
    int yAOpposite = static_cast<int>(posAOpposite.y * PHYSICS_SCALE);

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

void Cat::renderCatEars(SDL_Renderer* renderer) {
    // Render cat ears as triangles using filledTrigonRGBA
    // Triangles should be bound to particles at index NUM_CAT_PARTICLES/6 and 5*NUM_CAT_PARTICLES/6
    // The triangles should be filled with pink
    // The outline of the triangles should be drawn in black

    if (particles.empty()) return;
    b2Body* bodyA = particles[4*NUM_CAT_PARTICLES/6];
    b2Body* bodyB = particles[5*NUM_CAT_PARTICLES/6];
    b2Vec2 posA = bodyA->GetPosition();
    b2Vec2 posB = bodyB->GetPosition();

    b2Body* bodyAOpposite = particles[1*NUM_CAT_PARTICLES/6];
    b2Body* bodyBOpposite = particles[2*NUM_CAT_PARTICLES/6];

    // Get angle of the line between bodyA and boddyAOpposite
    b2Vec2 posAOpposite = bodyAOpposite->GetPosition();
    b2Vec2 posBOpposite = bodyBOpposite->GetPosition();
    float angle = atan2(posAOpposite.y - posA.y, posAOpposite.x - posA.x) - M_PI/6 + M_PI/3;
    float angle2 = atan2(posBOpposite.y - posB.y, posBOpposite.x - posB.x) - M_PI/6 + M_PI;

    int xA = static_cast<int>(posA.x * PHYSICS_SCALE);
    int yA = static_cast<int>(posA.y * PHYSICS_SCALE);
    int xB = static_cast<int>(posB.x * PHYSICS_SCALE);
    int yB = static_cast<int>(posB.y * PHYSICS_SCALE);

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
}