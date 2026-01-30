#include "PauseState.h"
#include "Game.h"

PauseState::PauseState()
    : font_(nullptr) {

    auto& rm = ResourceManager::getInstance();

    if (rm.hasFont("default")) {
        font_ = &rm.getFont("default");
    }

    overlay_.setSize(sf::Vector2f(1280.0f, 720.0f));
    overlay_.setFillColor(sf::Color(0, 0, 0, 180));

    if (font_) {
        pauseText_.setFont(*font_);
        pauseText_.setString("PAUSED");
        pauseText_.setCharacterSize(80);
        pauseText_.setFillColor(sf::Color(255, 215, 0));
        pauseText_.setStyle(sf::Text::Bold);
        pauseText_.setPosition(640.0f - pauseText_.getGlobalBounds().width / 2.0f, 250.0f);

        resumeText_.setFont(*font_);
        resumeText_.setString("Press P or ESC to Resume");
        resumeText_.setCharacterSize(40);
        resumeText_.setFillColor(sf::Color::White);
        resumeText_.setPosition(640.0f - resumeText_.getGlobalBounds().width / 2.0f, 400.0f);
    }
}

void PauseState::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) {
            if (game_) {
                game_->popState();
            }
        }
    }
}

void PauseState::update(float deltaTime) {
    // Pause state doesn't need updates
}

void PauseState::render(sf::RenderWindow& window) {
    window.draw(overlay_);
    if (font_) {
        window.draw(pauseText_);
        window.draw(resumeText_);
    }
}

