
#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <vector>

class HUD {
public:
    HUD();

    void update(int score,
        int highScore,
        float distance,
        int combo,
        float multiplier,
        bool shieldActive,
        bool magnetActive,
        bool doublePointsActive,
        int lives);
    void render(sf::RenderWindow& window);

    void setScore(int score);
    void animateScoreUpdate();
    void updateLives(int lives);

private:
    void updateLifeIcons();
    sf::Text scoreText_;
    sf::Text livesText_;
    sf::Text highScoreText_;
    sf::Text distanceText_;
    sf::Text comboText_;
    sf::Text multiplierText_;
    sf::Text powerUpText_;
    int currentScore_;
    int displayScore_;
    float scoreAnimationTime_;
    bool animatingScore_;
    int currentLives_;
    std::vector<sf::Sprite> lifeIcons_;

    sf::Font* font_;
};

/*
#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <vector>  // Add this for lifeIcons_

class HUD {
public:
    HUD();

    void update(int score,
        int highScore,
        float distance,
        int combo,
        float multiplier,
        bool shieldActive,
        bool magnetActive,
        bool doublePointsActive,
        int lives);  // Add lives parameter

    void render(sf::RenderWindow& window);

    void setScore(int score);
    void animateScoreUpdate();
    void updateLives(int lives);  // Add this method

private:
    void updateLifeIcons();  // Helper method for updating icons

    sf::Text scoreText_;
    sf::Text livesText_;  // Add this
    sf::Text highScoreText_;
    sf::Text distanceText_;
    sf::Text comboText_;
    sf::Text multiplierText_;
    sf::Text powerUpText_;

    int currentScore_;
    int displayScore_;
    int currentLives_;  // Add this
    float scoreAnimationTime_;
    bool animatingScore_;

    std::vector<sf::Sprite> lifeIcons_;  // For displaying life icons

    sf::Font* font_;
};
*/