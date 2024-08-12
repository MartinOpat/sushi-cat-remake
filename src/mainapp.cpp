#include "mainapp.h"

MainApp::MainApp(int level) {
    // ----------------- Level selection ----------------- 
    currLvl = new lvl(level);

    // ----------------- Initialize Graphics -----------------
    view = new MainView();
    view->setBackground(currLvl->bgTexPath);
    view->setBoxTexture("icons/box.png");

    // ----------------- Make world base -----------------
    world = new World();
    cat = nullptr;

    // Create 4 walls around the screen to prevent objects from moving out of the screen
    // Left wall
    walls.push_back(new Wall(0, WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE))); 
    // Top wall
    walls.push_back(new Wall(WINDOW_WIDTH/SCALE, 0, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE, 0))); 
    // Right wall
    walls.push_back(new Wall(0, WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE, 0))); 
    // Bottom wall -> Will be replaced by boxes
    // walls.push_back(new Wall(WINDOW_WIDTH/SCALE, 0, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE)));

    // Create 4 equidistant static rod obstacles at the bottom of the screen to simulate box edges
    float rodWidth = BOX_OFFSET/SCALE;
    walls.push_back(new Wall(rodWidth/2, 0.09f*WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE)));
    walls.push_back(new Wall(rodWidth, 0.09f*WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE/4, WINDOW_HEIGHT/SCALE)));
    walls.push_back(new Wall(rodWidth, 0.09f*WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE/2, WINDOW_HEIGHT/SCALE)));
    walls.push_back(new Wall(rodWidth, 0.09f*WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(3*WINDOW_WIDTH/SCALE/4, WINDOW_HEIGHT/SCALE)));
    walls.push_back(new Wall(rodWidth/2, 0.09f*WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE, WINDOW_HEIGHT/SCALE)));
    

    // ----------------- Level loading ----------------- 
    // Init cats
    catsLeft = currLvl->catCount;

    // Create sushis
    for (b2Vec2& pos : currLvl->posSushis) {
        sushis.push_back(new Sushi(pos/SCALE, world->getb2World()));
    }

    // Create kinematic obstacles
    if (currLvl->hasKinObs) {
        for (size_t i = 0; i < currLvl->posKinObs.size(); i++){
            kinematicObstacles.push_back(new Obstacle((currLvl->vertKinObs)[i],
                world->getb2World(),
                (currLvl->posKinObs)[i],
                b2_kinematicBody, 
                false, 
                1.0f));
        }
    }

    // Create static obstacles
    if (currLvl->hasStatObs) {
        for (size_t i = 0; i < currLvl->posStatObs.size(); i++){
            staticObstacles.push_back(new Obstacle((currLvl->vertStatObs)[i],
                world->getb2World(),
                (currLvl->posStatObs)[i],
                b2_staticBody, 
                true));
        }
    }

}

MainApp::~MainApp() {
    for (Wall* wall : walls) {
        world->getb2World()->DestroyBody(wall->getb2Body());
        delete wall;
    }
    for (Obstacle* obstacle : kinematicObstacles) {
        world->getb2World()->DestroyBody(obstacle->getb2Body());
        delete obstacle;
    }
    for (Obstacle* obstacle : staticObstacles) {
        world->getb2World()->DestroyBody(obstacle->getb2Body());
        delete obstacle;
    }
    for (Obstacle* obstacle : dynamicObstacles) {
        world->getb2World()->DestroyBody(obstacle->getb2Body());
        delete obstacle;
    }
    for (Sushi* sushi : sushis) {
        world->getb2World()->DestroyBody(sushi->getb2Body());
        delete sushi;
    }
    if (cat != nullptr) {
        cat->destroy(world->getb2World());
        delete cat;
    }
    delete world;
    delete view;
}

void MainApp::run() {
    const int FPS = 60;
    const int frameDelay = 1000 / FPS; // Milliseconds per frame
    Uint32 frameStart;
    int frameTime;

    while (!quit) {
        frameStart = SDL_GetTicks();

        pollEvents();
        handlePhysics();
        advanceTime();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

// ----------------- Physics handling -----------------

void MainApp::handlePhysics() {
    handleCollisions();
    handleCatStuck();
}

void MainApp::handleCollisions() {
    if (cat == nullptr) return;
    std::set<Sushi*> sushiesToRemove;
    for (auto* particle : cat->getParticles()) {
        for (auto* sushi : sushis) {
            b2Body* sushiBody = sushi->getb2Body();
            if (b2TestOverlap(particle->GetFixtureList()->GetShape(), 0, sushiBody->GetFixtureList()->GetShape(), 0, particle->GetTransform(), sushiBody->GetTransform())) {
                sushiesToRemove.insert(sushi);
            }
        }
    }
    // std::cout << "Collisions: " << bodiesToRemove.size() << std::endl;

    // Remove the sushi bodies that were touched
    for (Sushi* sushi : sushiesToRemove) {
        world->getb2World()->DestroyBody(sushi->getb2Body());
        sushis.remove(sushi);
        delete sushi;

        // Make circle bigger by lengthening the distance joints and increasing radius
        cat->eatSushi();
    }
}

void MainApp::handleCatStuck() {
    static bool catWasStuck = false;
    if (cat != nullptr && cat->isStuck()) {
        std::cout << "Cat is stuck!" << std::endl;
        cat->toggleSquish(world->getb2World());
        catWasStuck = true;
    }

    // Untoggle stuck cat after 3 seconds
    if (catWasStuck) {
        static std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
        std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
        std::chrono::duration<float> elapsed_seconds = now - last;
        if (elapsed_seconds.count() > 3) {
            cat->toggleSquish(world->getb2World());
            catWasStuck = false;
            last = now;
        }
    }
}

// ----------------- Time handling -----------------


void MainApp::advanceTime() {
    static std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed_seconds = now - last;
    last = now;
    float dt = elapsed_seconds.count();
    world->getb2World()->Step(dt, 6, 2); // Step the world forward
}

// ----------------- Rendering -----------------

void MainApp::render() {
    view->renderBackground();

    // renderWalls();
    renderObstacles();
    renderSushis();
    renderCat();
    renderUI();

    view->renderBoxes();

    SDL_RenderPresent(view->getRenderer());
}

void MainApp::renderWalls() {
    for (Wall* wall : walls) {
        wall->render(view->getRenderer());
    }
}

void MainApp::renderObstacles() {
    for (Obstacle* obstacle : kinematicObstacles) {
        obstacle->render(view->getRenderer());
    }
    for (Obstacle* obstacle : staticObstacles) {
        obstacle->render(view->getRenderer());
    }
    for (Obstacle* obstacle : dynamicObstacles) {
        obstacle->render(view->getRenderer());
    }
}

void MainApp::renderCat() {
    if (cat != nullptr)
        cat->render(view->getRenderer());
}

void MainApp::renderSushis() {
    for (Sushi* sushi : sushis) {
        sushi->render(view->getRenderer());
    }
}

void MainApp::renderUI() {
    // ----------------- Render every frame UI
    view->renderSushiLeftUI(sushis.size());
    view->renderCatsLeftUI(catsLeft);

    // ----------------- Render end of LVL UI
    if (sushis.size() == 0) {
        view->renderLevelWonUI();
    } else if (catsLeft <= 0 && cat == nullptr) {
        view->renderLevelLostUI();
    }

    // ----------------- Render end of run UI
    if (isFirstCat) return;
    static std::chrono::time_point<std::chrono::system_clock> last = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    if (isNewCat){
        last = now;
        isNewCat = false;
    } 
    std::chrono::duration<float> elapsed_seconds = now - last;
    if (elapsed_seconds.count() < 2) {
        view->renderSushiEatenUI(lastCatSushiEaten);
    }
}

// ----------------- Event handling -----------------

void MainApp::handleMouseClick(int x, int y) {
    x = std::max(x, BASE_CAT_RADIUS);
    x = std::min(x, (int)(WINDOW_WIDTH/SCALE) - BASE_CAT_RADIUS);
    y = CAT_SPAWN_HEIGHT;

    cat = new Cat(x, y, world->getb2World());
    newCatReady = false;
    catsLeft--;
}

void MainApp::handleKeyPressSpace() {
    if (cat != nullptr) {
        lastCatSushiEaten = cat->getEatenSushis();
        cat->destroy(world->getb2World());
        delete cat;
        isNewCat = true;
        isFirstCat = false;
        newCatReady = true;
    }
    cat = nullptr;
}

void MainApp::handleKeyPress(SDL_Keycode key) {
    if (key == SDLK_SPACE) {
        handleKeyPressSpace();
    } else if (key == SDLK_ESCAPE) {
        quit = true;
    }
}

void MainApp::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int x, y;
            x = event.button.x / SCALE;
            y = event.button.y / SCALE;

            if (!newCatReady || catsLeft <= 0) return;
            handleMouseClick(x, y);
            isDragging = true;
        } else if  (event.type == SDL_MOUSEBUTTONUP) {
            isDragging = false;
        } else if (event.type == SDL_KEYDOWN) {
            handleKeyPress(event.key.keysym.sym);
        }
    }

    if (isDragging) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        float newX = x / SCALE;
        newX = std::max(newX, (float)BASE_CAT_RADIUS);
        newX = std::min(newX, WINDOW_WIDTH/SCALE - BASE_CAT_RADIUS);
        cat->setPosition(newX, CAT_SPAWN_HEIGHT);
    }

    if (cat != nullptr && cat->isOffscreen()) {
        handleKeyPressSpace();
    }
}





