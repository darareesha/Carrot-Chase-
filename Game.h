#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <stack>
#include "GameState.h"
#include "ResourceManager.h"

class Game {
public:
    Game();
    ~Game();

    void run();
    void changeState(std::unique_ptr<GameState> state);
    void pushState(std::unique_ptr<GameState> state);
    void popState();

    sf::RenderWindow& getWindow() { return window_; }
    ResourceManager& getResourceManager() { return ResourceManager::getInstance(); }

    bool isRunning() const { return running_; }
    void quit() { running_ = false; }

private:
    void handleEvents();
    void update(float deltaTime);
    void render();

    sf::RenderWindow window_;
    sf::Clock clock_;
    float deltaTime_;

    std::stack<std::unique_ptr<GameState>> states_;
    bool running_;

    sf::Music backgroundMusic_;
};



