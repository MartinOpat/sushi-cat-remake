#include "lvl.h"

lvl::lvl(int num) {
    switch (num) {
        case 1:
            // First column
            posSushis.push_back(b2Vec2(300, 400));
            posSushis.push_back(b2Vec2(300, 700));
            posSushis.push_back(b2Vec2(300, 1000));

            posKinematicObstacles.push_back(b2Vec2(450, 300));


            // Second column
            posSushis.push_back(b2Vec2(600, 400));
            posSushis.push_back(b2Vec2(600, 700));
            posSushis.push_back(b2Vec2(600, 1000));


            // Third column
            posSushis.push_back(b2Vec2(900, 400));
            posSushis.push_back(b2Vec2(900, 700));
            posSushis.push_back(b2Vec2(900, 1000));

            posKinematicObstacles.push_back(b2Vec2(1050, 300));

            // Fourth column
            posSushis.push_back(b2Vec2(1200, 400));
            posSushis.push_back(b2Vec2(1200, 700));
            posSushis.push_back(b2Vec2(1200, 1000));



            kinematicObstacleWidth = 200;
            kinematicObstacleHeight = 25;
            break;

        case 2:
            // Windmill in the middle
            posKinematicObstacles.push_back(b2Vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
            kinematicObstacleHeight = 200;
            kinematicObstacleWidth = 200;

        default:
            break;
    }

}