#include "lvl.h"

lvl::lvl(int num) {
    switch (num) {
        case 1:
            posSushis.push_back(b2Vec2(300, 400));
            posSushis.push_back(b2Vec2(300, 700));
            posSushis.push_back(b2Vec2(300, 1000));
            


            posObstacles.push_back(b2Vec2(350, 300));
       

            obstacleWidth = 200;
            obstacleHeight = 25;
            break;

        default:
            break;
    }

}