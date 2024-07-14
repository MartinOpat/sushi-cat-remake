#ifndef LVL_H
#define LVL_H

#include "../box2d-main/include/box2d/box2d.h"
#include "globals.h"

#include <vector>


class lvl {
public:
    lvl(int num);
    
    std::vector<b2Vec2> posSushis;
    std::vector<b2Vec2> posKinematicObstacles;
    float kinematicObstacleWidth;
    float kinematicObstacleHeight;
};

#endif // LVL_H