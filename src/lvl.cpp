#include "lvl.h"

lvl::lvl(int num) {
    switch (num) {
        case 1:
            // First column
            posSushis.push_back(b2Vec2(300, 400));
            posSushis.push_back(b2Vec2(300, 700));
            posSushis.push_back(b2Vec2(300, 1000));

            posObstacles.push_back(b2Vec2(450, 300));


            // Second column
            posSushis.push_back(b2Vec2(600, 400));
            posSushis.push_back(b2Vec2(600, 700));
            posSushis.push_back(b2Vec2(600, 1000));


            // Third column
            posSushis.push_back(b2Vec2(900, 400));
            posSushis.push_back(b2Vec2(900, 700));
            posSushis.push_back(b2Vec2(900, 1000));

            posObstacles.push_back(b2Vec2(1050, 300));

            // Fourth column
            posSushis.push_back(b2Vec2(1200, 400));
            posSushis.push_back(b2Vec2(1200, 700));
            posSushis.push_back(b2Vec2(1200, 1000));



            obstacleWidth = 200;
            obstacleHeight = 25;
            break;

        default:
            break;
    }

}