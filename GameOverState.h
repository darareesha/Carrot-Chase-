#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>

class GameOverState : public GameState {
public:
    GameOverState(int finalScore, int highScore, float distance);

    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    int finalScore_;
    int highScore_;
    float distance_;
    sf::Text gameOverText_;
    sf::Text scoreText_;
    sf::Text bestScoreText_;
    sf::Text distanceText_;
    sf::Text restartText_;
    sf::Text menuText_;
    sf::Font* font_;
    sf::RectangleShape background_;
};

