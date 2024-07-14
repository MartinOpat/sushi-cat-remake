#include "mainapp.h"

MainApp::MainApp() {
    // ----------------- Level selection ----------------- 
    currLvl = new lvl(1);

    // ----------------- Initialize Graphics -----------------
    view = new MainView();
    view->setBackground(currLvl->bgTexPath);

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
    // Bottom wall
    walls.push_back(new Wall(WINDOW_WIDTH/SCALE, 0, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE)));


    // ----------------- Level loading ----------------- 
    // Create sushis
    for (b2Vec2& pos : currLvl->posSushis) {
        sushis.push_back(new Sushi(pos/SCALE, world->getb2World()));
    }

    // Create kinematic obstacles
    for (size_t i = 0; i < currLvl->posKinObs.size(); i++){
        kinematicObstacles.push_back(new Obstacle((currLvl->vertKinObs)[i],
            world->getb2World(),
            (currLvl->posKinObs)[i],
            b2_kinematicBody, 
            false, 
            1.0f));
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
        handleCollisions();
        advanceTime();
        render();

        frameTime = SDL_GetTicks() - frameStart;
        if (frameDelay > frameTime) {
            SDL_Delay(frameDelay - frameTime);
        }
    }
}

// ----------------- Collision handling -----------------

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
    // SDL_SetRenderDrawColor(view->getRenderer(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    // SDL_RenderClear(view->getRenderer());
    view->renderBackground();

    renderWalls();
    renderObstacles();
    renderSushis();
    renderCat();
    renderUI();

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

    // ----------------- Render end of LVL UI
    if (sushis.size() == 0) {
        view->renderLevelWonUI();
    }

    // ----------------- Render end of run UI
    if (cat == nullptr || isFirstCat) return;
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
    // y = std::max(y, BASE_CAT_RADIUS);
    y = CAT_SPAWN_HEIGHT;

    if (cat != nullptr) {
        lastCatSushiEaten = cat->getEatenSushis();
        cat->destroy(world->getb2World());
        delete cat;
        isNewCat = true;
        isFirstCat = false;
    }
    cat = new Cat(x, y, world->getb2World());
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
            handleMouseClick(x, y);
            isDragging = true;
        } else if  (event.type == SDL_MOUSEBUTTONUP) {
            isDragging = false;
        }
    }

    if (isDragging) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        float newX = x / SCALE;
        newX = std::max(newX, (float)BASE_CAT_RADIUS);
        cat->setPosition(newX, CAT_SPAWN_HEIGHT);
    }
}





