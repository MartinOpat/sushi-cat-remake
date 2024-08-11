#include "lvl.h"

lvl::lvl(int num) {
    switch (num) {
        case 1:
            // ----------------- Cat -----------------
            catCount = 1;

            // ----------------- Background -----------------
            #ifdef _WIN32
                bgTexPath = "icons\\bg1.jpg";
            #else
                bgTexPath = "icons/bg1.jpg";
            #endif

            // ----------------- Sushi -----------------
            // One long column in the middle of the screen
            for (int i = 0; i < 8; i++) {
                posSushis.push_back(b2Vec2(WINDOW_WIDTH/2, 100 + (i+1)*100));
            }

            break;

        case 2:
            // ----------------- Cat -----------------
            catCount = 4;

            // ----------------- Background -----------------
            #ifdef _WIN32
                bgTexPath = "icons\\bg1.jpg";
            #else
                bgTexPath = "icons/bg1.jpg";
            #endif

            // ----------------- Static Obstacles -----------------
            // Make a simple funnel (two angled diagonals)
            hasStatObs = true;
            statObsWidth = 200;
            statObsHeight = 25;
            
            // Left diagonal
            posStatObs.push_back(b2Vec2(600, 400) / SCALE);
            vertStatObs.push_back({
                b2Vec2(-500, -200) / SCALE,
                b2Vec2(300, 200-20) / SCALE,
                b2Vec2(300, 200) / SCALE,
                b2Vec2(-500, -200+20) / SCALE
            });

            // Right diagonal
            posStatObs.push_back(b2Vec2(1325, 400) / SCALE);
            vertStatObs.push_back({
                b2Vec2(-300, 200-20) / SCALE,
                b2Vec2(500, -200) / SCALE,
                b2Vec2(500, -200+20) / SCALE,
                b2Vec2(-300, 200) / SCALE
            });

            // ----------------- Kinematic Obstacles -----------------
            // Rotating windmill in the middle of the screen
            hasKinObs = true;
            kinObsWidth = 200;
            kinObsHeight = 25;

            posKinObs.push_back(b2Vec2(WINDOW_WIDTH/2, 3*WINDOW_HEIGHT/4) / SCALE);
            vertKinObs.push_back({
                b2Vec2(-100, -10) / SCALE,
                b2Vec2(100, -10) / SCALE,
                b2Vec2(100, 10) / SCALE,
                b2Vec2(-100, 10) / SCALE
            });

            // ----------------- Sushi -----------------
            // One column above the windmill
            for (int i = 0; i < 5; i++) {
                posSushis.push_back(b2Vec2(WINDOW_WIDTH/2, 200 + (i+1)*100));
            }

            // Two rows matching the angle of the funnel (i.e., lying on the sides of the funnel diagonals)
            for (int i = 0; i < 4; i++) {
                posSushis.push_back(b2Vec2(300 + i*150, 200 + i*75));
                posSushis.push_back(b2Vec2(1625 - i*150, 200 + i*75));
            }

            break;

        case 3:
            // ----------------- Cat -----------------
            catCount = 9;

            // ----------------- Background -----------------
            #ifdef _WIN32
                bgTexPath = "icons\\bg1.jpg";
            #else
                bgTexPath = "icons/bg1.jpg";
            #endif

            // ----------------- Sushi -----------------
            // First column
            posSushis.push_back(b2Vec2(300, 400));
            posSushis.push_back(b2Vec2(300, 700));

            // Second column
            posSushis.push_back(b2Vec2(600, 400));
            posSushis.push_back(b2Vec2(600, 700));


            // Third column
            posSushis.push_back(b2Vec2(900, 400));
            posSushis.push_back(b2Vec2(900, 700));

            // Fourth column
            posSushis.push_back(b2Vec2(1200, 400));
            posSushis.push_back(b2Vec2(1200, 700));

            // Fifth column
            posSushis.push_back(b2Vec2(1500, 400));
            posSushis.push_back(b2Vec2(1500, 700));

            // ----------------- Kinematic Obstacles -----------------
            hasKinObs = true;
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
            
            posKinObs.push_back(b2Vec2(1600, 500) / SCALE);
            vertKinObs.push_back({
                b2Vec2(-100, -10) / SCALE,
                b2Vec2(100, -10) / SCALE,
                b2Vec2(100, 10) / SCALE,
                b2Vec2(-100, 10) / SCALE
            });

            break;
        default:
            break;
    }

}