#include "mainapp.h"

MainApp::MainApp() {
    view = new MainView();
    world = new World();
    cat = nullptr;

    // Create 4 walls around the screen to prevent objects from moving out of the screen
    walls.push_back(new Wall(0, WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE))); // Left wall
    walls.push_back(new Wall(WINDOW_WIDTH/SCALE, 0, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE, 0))); // Top wall
    walls.push_back(new Wall(0, WINDOW_HEIGHT/SCALE, world->getb2World(), b2Vec2(WINDOW_WIDTH/SCALE, 0))); // Right wall
    walls.push_back(new Wall(WINDOW_WIDTH/SCALE, 0, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT/SCALE))); // Bottom wall

    // Create an obstacle in the middle of the screen
    obstacles.push_back(new Obstacle({
        b2Vec2(WINDOW_WIDTH/SCALE/2, WINDOW_HEIGHT/SCALE/2),
        b2Vec2(WINDOW_WIDTH/SCALE/2 + 50/SCALE, WINDOW_HEIGHT/SCALE/2),
        b2Vec2(WINDOW_WIDTH/SCALE/2 + 50/SCALE, WINDOW_HEIGHT/SCALE/2 + 50/SCALE),
        b2Vec2(WINDOW_WIDTH/SCALE/2, WINDOW_HEIGHT/SCALE/2 + 50/SCALE)
    }, world->getb2World(), b2Vec2(0, 0), b2_staticBody));


}

MainApp::~MainApp() {
    delete view;
    delete world;

    for (Wall* wall : walls) {
        delete wall;
    }
}

void MainApp::run() {
    while (!quit) {
        pollEvents();
        advanceTime();
        render();
    }
}

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
    SDL_SetRenderDrawColor(view->getRenderer(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(view->getRenderer());

    renderWalls();
    renderObstacles();
    renderCat();

    SDL_RenderPresent(view->getRenderer());
}

void MainApp::renderWalls() {
    for (Wall* wall : walls) {
        wall->render(view->getRenderer());
    }
}

void MainApp::renderObstacles() {
    for (Obstacle* obstacle : obstacles) {
        obstacle->render(view->getRenderer());
    }
}

void MainApp::renderCat() {
    if (cat != nullptr)
        cat->render(view->getRenderer());
}


// ----------------- Event handling -----------------

void MainApp::handleMouseClick(int x, int y) {
    x = std::max(x, BASE_CAT_RADIUS);
    // y = std::max(y, BASE_CAT_RADIUS);
    y = CAT_SPAWN_HEIGHT;

    if (cat != nullptr) {
        cat->destroy(world->getb2World());
        delete cat;
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



