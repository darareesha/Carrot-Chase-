
#include "HUD.h"
#include <sstream>
#include <iomanip>

HUD::HUD()
    : currentScore_(0)
    , displayScore_(0)
    , currentLives_(3)
    , scoreAnimationTime_(0.0f)
    , animatingScore_(false)
    , font_(nullptr) {

    auto& rm = ResourceManager::getInstance();

    if (rm.hasFont("default")) {
        font_ = &rm.getFont("default");
    }

    auto setup = [&](sf::Text& text, unsigned size, const sf::Color& fill, const sf::Color& outline, sf::Vector2f pos) {
        if (!font_) return;
        text.setFont(*font_);
        text.setCharacterSize(size);
        text.setFillColor(fill);
        text.setOutlineColor(outline);
        text.setOutlineThickness(2.0f);
        text.setPosition(pos);
        text.setStyle(sf::Text::Bold);
        };

    setup(scoreText_, 48, sf::Color(255, 215, 0), sf::Color(139, 69, 19), { 20.0f, 15.0f });
    // livesText_ position set in updateLives
    setup(highScoreText_, 32, sf::Color::White, sf::Color(70, 130, 180), { 20.0f, 110.0f });
    setup(distanceText_, 32, sf::Color(173, 216, 230), sf::Color(25, 25, 112), { 20.0f, 150.0f });
    setup(comboText_, 32, sf::Color(255, 182, 193), sf::Color(255, 105, 180), { 20.0f, 190.0f });
    setup(multiplierText_, 32, sf::Color(144, 238, 144), sf::Color(34, 139, 34), { 20.0f, 230.0f });
    setup(powerUpText_, 28, sf::Color::White, sf::Color(123, 104, 238), { 20.0f, 270.0f });
    updateLifeIcons();
}

void HUD::update(int score,
    int highScore,
    float distance,
    int combo,
    float multiplier,
    bool shieldActive,
    bool magnetActive,
    bool doublePointsActive,
    int lives) {
    currentScore_ = score;

    if (displayScore_ < currentScore_) {
        animatingScore_ = true;
        int diff = currentScore_ - displayScore_;
        int increment = std::max(1, diff / 10);
        displayScore_ = std::min(displayScore_ + increment, currentScore_);
    }
    else {
        animatingScore_ = false;
    }

    if (!font_) return;

    std::stringstream ss;
    ss << "Score: " << std::setfill('0') << std::setw(6) << displayScore_;
    scoreText_.setString(ss.str());

    std::stringstream hs;
    hs << "Best: " << highScore;
    highScoreText_.setString(hs.str());

    std::stringstream ds;
    ds << "Distance: " << std::fixed << std::setprecision(1) << distance << " m";
    distanceText_.setString(ds.str());

    if (combo > 1) {
        comboText_.setString("Combo x" + std::to_string(combo));
    }
    else {
        comboText_.setString("");
    }

    if (multiplier > 1.0f) {
        multiplierText_.setString("Multiplier x" + std::to_string(static_cast<int>(multiplier)));
    }
    else {
        multiplierText_.setString("");
    }

    std::string powerText;
    if (shieldActive) powerText += "[Shield] ";
    if (magnetActive) powerText += "[Magnet] ";
    if (doublePointsActive) powerText += "[Double Points]";
    powerUpText_.setString(powerText);

    updateLives(lives);
}

void HUD::render(sf::RenderWindow& window) {
    if (!font_) return;
    window.draw(scoreText_);
    window.draw(livesText_);
    window.draw(highScoreText_);
    window.draw(distanceText_);
    if (!comboText_.getString().isEmpty()) window.draw(comboText_);
    if (!multiplierText_.getString().isEmpty()) window.draw(multiplierText_);
    if (!powerUpText_.getString().isEmpty()) window.draw(powerUpText_);
    for (const auto& icon : lifeIcons_) {
        window.draw(icon);
    }
}

void HUD::setScore(int score) {
    currentScore_ = score;
    displayScore_ = score;
}

void HUD::animateScoreUpdate() {
    animatingScore_ = true;
    scoreAnimationTime_ = 0.0f;
}

void HUD::updateLives(int lives) {
    currentLives_ = lives;
    if (font_) {
        livesText_.setString("Lives: " + std::to_string(currentLives_));
        livesText_.setPosition(1050.0f, 20.0f);
        updateLifeIcons();
    }
}

void HUD::updateLifeIcons() {
    lifeIcons_.clear();
    auto& rm = ResourceManager::getInstance();
    if (!rm.hasTexture("lifeline_icon")) return;

    // Always show 3 icons, faded if lost
    float iconX = 1200.0f;
    float iconY = 20.0f;
    float iconSpacing = 36.0f; // more spacing
    float iconScale = 0.32f;   // smaller size
    for (int i = 0; i < 3; ++i) {
        sf::Sprite icon;
        icon.setTexture(rm.getTexture("lifeline_icon"));
        icon.setScale(iconScale, iconScale);
        icon.setPosition(iconX - i * iconSpacing, iconY);
        if (i >= currentLives_) {
            icon.setColor(sf::Color(255,255,255,80)); // faded for lost life
        }
        lifeIcons_.push_back(icon);
    }
}

