#pragma once

#include "GameState.h"
#include <SFML/Graphics.hpp>

class MenuState : public GameState {
public:
    MenuState();

    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    sf::Text titleText_;
    sf::Text playText_;
    sf::Text exitText_;
    sf::Text instructionsText_;
    sf::Text startText_;

    int selectedOption_;
    sf::Font* font_;

    sf::RectangleShape background_;
    sf::Sprite playButtonSprite_;
    sf::Sprite exitButtonSprite_;
    sf::Sprite cursorSprite_;
    sf::Sprite bunnySprite_;
    bool useCustomCursor_ = false;
};
