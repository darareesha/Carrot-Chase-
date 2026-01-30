#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>

class PauseState : public GameState {
public:
    PauseState();
    
    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;
    
private:
    sf::RectangleShape overlay_;
    sf::Text pauseText_;
    sf::Text resumeText_;
    sf::Font* font_;
};

