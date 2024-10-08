#ifndef LVL_H
#define LVL_H

#include <box2d/box2d.h>
#include "globals.h"

#include <vector>
#include <string>

class lvl {
public:
    lvl(int num);
    
    // ----------------- Sushi cat -----------------
    std::vector<b2Vec2> posSushis;
    int catCount;

    // ----------------- Kinematic Obstacles -----------------
    bool hasKinObs = false;
    std::vector<b2Vec2> posKinObs;
    std::vector<std::vector<b2Vec2>> vertKinObs;
    float kinObsWidth;
    float kinObsHeight;

    // ----------------- Static Obstacles -----------------
    bool hasStatObs = false;
    std::vector<b2Vec2> posStatObs;
    std::vector<std::vector<b2Vec2>> vertStatObs;
    float statObsWidth;
    float statObsHeight;

    // ----------------- Textures -----------------
    std::string bgTexPath = "";
};

#endif // LVL_H