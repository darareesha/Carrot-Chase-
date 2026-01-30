#pragma once

#include <SFML/Graphics.hpp>

class Game;

class GameState {
public:
    virtual ~GameState() = default;
    
    virtual void handleInput(sf::Event& event) = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;
    
    void setGame(Game* game) { game_ = game; }
    
protected:
    Game* game_ = nullptr;
};

