#include "GameOverState.h"
#include "Game.h"
#include "MenuState.h"
#include "PlayingState.h"
#include <sstream>
#include <iomanip>

GameOverState::GameOverState(int finalScore, int highScore, float distance)
    : finalScore_(finalScore)
    , highScore_(highScore)
    , distance_(distance)
    , font_(nullptr) {

    auto& rm = ResourceManager::getInstance();

    if (rm.hasFont("default")) {
        font_ = &rm.getFont("default");
    }

    background_.setSize(sf::Vector2f(1280.0f, 720.0f));
    // Light brown: RGB(210, 180, 140) with some transparency
    background_.setFillColor(sf::Color(210, 180, 140, 200)); // Light brown with transparency



    if (font_) {
        gameOverText_.setFont(*font_);
        gameOverText_.setString("Game Over!");
        gameOverText_.setCharacterSize(80);
        // Dark brown: RGB(101, 67, 33)
        gameOverText_.setFillColor(sf::Color(101, 67, 33));
        gameOverText_.setOutlineColor(sf::Color(139, 69, 19));
        gameOverText_.setOutlineThickness(4.0f);
        gameOverText_.setStyle(sf::Text::Bold);
        gameOverText_.setPosition(640.0f - gameOverText_.getGlobalBounds().width / 2.0f, 180.0f);

        std::stringstream ss;
        ss << "Final Score: " << std::setfill('0') << std::setw(6) << finalScore_;
        scoreText_.setFont(*font_);
        scoreText_.setString(ss.str());
        scoreText_.setCharacterSize(60);
        scoreText_.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        scoreText_.setOutlineColor(sf::Color(139, 69, 19));
        scoreText_.setOutlineThickness(3.0f);
        scoreText_.setStyle(sf::Text::Bold);
        // Move score higher up (sky area)
        scoreText_.setPosition(640.0f - scoreText_.getGlobalBounds().width / 2.0f, 80.0f);

        bestScoreText_.setFont(*font_);
        bestScoreText_.setCharacterSize(40);
        bestScoreText_.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        bestScoreText_.setOutlineColor(sf::Color(139, 69, 19));
        std::stringstream bs;
        bs << "Best: " << highScore_;
        bestScoreText_.setString(bs.str());
        bestScoreText_.setPosition(640.0f - bestScoreText_.getGlobalBounds().width / 2.0f, 340.0f);

        distanceText_.setFont(*font_);
        distanceText_.setCharacterSize(36);
        distanceText_.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        distanceText_.setOutlineColor(sf::Color(139, 69, 19));
        std::stringstream ds;
        ds << "Distance: " << std::fixed << std::setprecision(1) << distance_ << " m";
        distanceText_.setString(ds.str());
        distanceText_.setPosition(640.0f - distanceText_.getGlobalBounds().width / 2.0f, 390.0f);

        restartText_.setFont(*font_);
        restartText_.setString("Press SPACE / ENTER to Play Again");
        restartText_.setCharacterSize(40);
        restartText_.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        restartText_.setOutlineColor(sf::Color(139, 69, 19));
        restartText_.setOutlineThickness(2.0f);
        restartText_.setPosition(640.0f - restartText_.getGlobalBounds().width / 2.0f, 460.0f);

        menuText_.setFont(*font_);
        menuText_.setString("Press ESC to Return to Menu");
        menuText_.setCharacterSize(35);
        menuText_.setFillColor(sf::Color(101, 67, 33)); // Dark brown
        menuText_.setOutlineColor(sf::Color(139, 69, 19));
        menuText_.setOutlineThickness(2.0f);
        menuText_.setPosition(640.0f - menuText_.getGlobalBounds().width / 2.0f, 580.0f);
    }
}

void GameOverState::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Space || event.key.code == sf::Keyboard::Enter) {
            if (game_) {
                auto state = std::make_unique<PlayingState>();
                state->setGame(game_);
                game_->changeState(std::move(state));
            }
        }
        else if (event.key.code == sf::Keyboard::Escape) {
            if (game_) {
                game_->changeState(std::make_unique<MenuState>());
            }
        }
    }
}

void GameOverState::update(float deltaTime) {
    // Animate text
    static float time = 0.0f;
    time += deltaTime;

    if (font_) {
        float scale = 1.0f + std::sin(time * 3.0f) * 0.03f;
        restartText_.setScale(scale, scale);
        // Keep it centered horizontally, fixed vertically for clean alignment
        restartText_.setPosition(640.0f - restartText_.getGlobalBounds().width / 2.0f, 460.0f);
    }
}

void GameOverState::render(sf::RenderWindow& window) {
    window.draw(background_);

    if (font_) {
        window.draw(gameOverText_);
        window.draw(scoreText_);
        window.draw(bestScoreText_);
        window.draw(distanceText_);
        window.draw(restartText_);
        window.draw(menuText_);
    }
}



