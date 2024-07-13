#include "mainapp.h"

MainApp::MainApp() {
    view = new MainView();
    world = new World();

    // Create 4 walls around the screen to prevent objects from moving out of the screen
    walls.push_back(new Wall(0, 0, world->getb2World(), b2Vec2(0, WINDOW_HEIGHT))); // Left wall
    walls.push_back(new Wall(0, 0, world->getb2World(), b2Vec2(WINDOW_WIDTH, 0))); // Top wall
    walls.push_back(new Wall(WINDOW_WIDTH, 0, world->getb2World(), b2Vec2(WINDOW_WIDTH, WINDOW_HEIGHT))); // Right wall
    walls.push_back(new Wall(0, WINDOW_HEIGHT, world->getb2World(), b2Vec2(WINDOW_WIDTH, WINDOW_HEIGHT))); // Bottom wall



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

void MainApp::render() {
    SDL_SetRenderDrawColor(view->getRenderer(), 100, 100, 100, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(view->getRenderer());

    renderWalls();

    SDL_RenderPresent(view->getRenderer());
}

void MainApp::renderWalls() {
    for (Wall* wall : walls) {
        wall->render(view->getRenderer());
    }
}

void MainApp::pollEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            quit = true;
        }
    }
}


