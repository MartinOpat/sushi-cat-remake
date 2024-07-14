#include "lvl.h"

lvl::lvl(int num) {
    switch (num) {
        case 1:
            // ----------------- Cat -----------------
            catCount = 6;

            // ----------------- Background -----------------
            bgTexPath = "icons/bg1.jpg";

            // ----------------- Sushi -----------------
            // First column
            posSushis.push_back(b2Vec2(300, 400));
            posSushis.push_back(b2Vec2(300, 700));
            posSushis.push_back(b2Vec2(300, 1000));

            // Second column
            posSushis.push_back(b2Vec2(600, 400));
            posSushis.push_back(b2Vec2(600, 700));
            posSushis.push_back(b2Vec2(600, 1000));


            // Third column
            posSushis.push_back(b2Vec2(900, 400));
            posSushis.push_back(b2Vec2(900, 700));
            posSushis.push_back(b2Vec2(900, 1000));

            // Fourth column
            posSushis.push_back(b2Vec2(1200, 400));
            posSushis.push_back(b2Vec2(1200, 700));
            posSushis.push_back(b2Vec2(1200, 1000));

            // ----------------- Kinematic Obstacles -----------------
            kinObsWidth = 200;
            kinObsHeight = 25;

            posKinObs.push_back(b2Vec2(400, 500) / SCALE);
            vertKinObs.push_back({
                b2Vec2(-100, -10) / SCALE,
                b2Vec2(100, -10) / SCALE,
                b2Vec2(100, 10) / SCALE,
                b2Vec2(-100, 10) / SCALE
            });

            posKinObs.push_back(b2Vec2(800, 500) / SCALE);
            vertKinObs.push_back({
                b2Vec2(-100, -10) / SCALE,
                b2Vec2(100, -10) / SCALE,
                b2Vec2(100, 10) / SCALE,
                b2Vec2(-100, 10) / SCALE
            });

            posKinObs.push_back(b2Vec2(1200, 500) / SCALE);
            vertKinObs.push_back({
                b2Vec2(-100, -10) / SCALE,
                b2Vec2(100, -10) / SCALE,
                b2Vec2(100, 10) / SCALE,
                b2Vec2(-100, 10) / SCALE
            });
            

            break;

        case 2:
            // Windmill in the middle
            // posKinematicObstacles.push_back(b2Vec2(WINDOW_WIDTH/2, WINDOW_HEIGHT/2));
            kinObsWidth = 200;
            kinObsHeight = 200;

        default:
            break;
    }

}