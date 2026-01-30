#include "Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <iostream>

Player::Player()
    : state_(PlayerState::Idle)
    , velocity_(0.0f, 0.0f)
    , position_(100.0f, 400.0f)
    , gravity_(1200.0f)
    , jumpStrength_(-800.0f)      
    , smallJumpStrength_(-450.0f) 
    , runSpeed_(150.0f)           
    , speedBoostMultiplier_(1.0f)
    , speedBoostTimer_(0.0f)
    , iceTimer_(0.0f)
    , slideDuration_(0.5f)
    , slideTimer_(0.0f)
    , grounded_(false)
    , facingRight_(true)
    , isSliding_(false)
    , score_(0)
    , spacePressed_(false)
    , downPressed_(false)
    , moveLeftHeld_(false)
    , moveRightHeld_(false)
    , maxJumps_(2)
    , jumpsUsed_(0) {

    loadAnimations();

    // Set initial sprite
    auto& rm = ResourceManager::getInstance();
    if (rm.hasTexture("player_idle")) {
        sprite_.setTexture(rm.getTexture("player_idle"));
        auto size = rm.getTexture("player_idle").getSize();
        sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }
    sprite_.setPosition(position_);
    sprite_.setScale(0.4f, 0.4f);

    // Setup sounds
    if (rm.hasSoundBuffer("jump")) {
        jumpSound_.setBuffer(rm.getSoundBuffer("jump"));
        jumpSound_.setVolume(50.0f);
    }
    if (rm.hasSoundBuffer("slide")) {
        slideSound_.setBuffer(rm.getSoundBuffer("slide"));
        slideSound_.setVolume(40.0f);
    }
    if (rm.hasSoundBuffer("death")) {
        deathSound_.setBuffer(rm.getSoundBuffer("death"));
        deathSound_.setVolume(60.0f);
    }
}

void Player::loadAnimations() {
    auto& rm = ResourceManager::getInstance();

    // Idle animation - single frame (bunny1_stand.png)
    if (rm.hasTexture("player_idle")) {
        auto& tex = rm.getTexture("player_idle");
        auto size = tex.getSize();
        idleAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        idleAnim_.setFrameTime(0.15f);
    }

    // Run animation - alternate between walk1 and walk2
    if (rm.hasTexture("player_run")) {
        auto& tex = rm.getTexture("player_run");
        auto size = tex.getSize();
        runAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        runAnim_.setFrameTime(0.15f);
    }
    // Load walk2 for run animation alternation
    if (rm.hasTexture("player_run2")) {
        auto& tex = rm.getTexture("player_run2");
        auto size = tex.getSize();
        runAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }

    // Jump animation - single frame (bunny1_jump.png)
    if (rm.hasTexture("player_jump")) {
        auto& tex = rm.getTexture("player_jump");
        auto size = tex.getSize();
        jumpAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        jumpAnim_.setFrameTime(0.12f);
        jumpAnim_.setLooping(false);
    }

    // Slide animation - single frame (bunny1_ready.png)
    if (rm.hasTexture("player_slide")) {
        auto& tex = rm.getTexture("player_slide");
        auto size = tex.getSize();
        slideAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        slideAnim_.setFrameTime(0.1f);
    }

    // Death animation - single frame (bunny1_hurt.png)
    if (rm.hasTexture("player_death")) {
        auto& tex = rm.getTexture("player_death");
        auto size = tex.getSize();
        deathAnim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        deathAnim_.setFrameTime(0.15f);
        deathAnim_.setLooping(false);
    }
}

void Player::update(float deltaTime) {
    if (state_ == PlayerState::Dead) {
        updateAnimation(deltaTime);
        // Apply gravity even when dead
        velocity_.y += gravity_ * deltaTime;
        position_.y += velocity_.y * deltaTime;
        sprite_.setPosition(position_);
        return;
    }

    // Update temporary effects
    if (speedBoostTimer_ > 0.0f) {
        speedBoostTimer_ -= deltaTime;
        if (speedBoostTimer_ <= 0.0f) {
            speedBoostMultiplier_ = 1.0f;
        }
    }
    if (iceTimer_ > 0.0f) {
        iceTimer_ = std::max(0.0f, iceTimer_ - deltaTime);
    }

    // Check keyboard state directly for continuous input (jump)
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Up) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        if (!spacePressed_ && !isSliding_) {
            jump();
            spacePressed_ = true;
        }
    }
    else {
        spacePressed_ = false;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        if (!downPressed_ && grounded_ && !isSliding_) {
            slide();
            downPressed_ = true;
        }
    }
    else {
        downPressed_ = false;
    }

    // Auto-run to the right
    if (!isSliding_ && state_ != PlayerState::Dead) {
        velocity_.x = getRunSpeed();
        facingRight_ = true;
    }
    else {
        // Stop when sliding or dead
        velocity_.x = 0.0f;
    }

    // Handle sliding
    if (isSliding_) {
        slideTimer_ -= deltaTime;
        if (slideTimer_ <= 0.0f) {
            isSliding_ = false;
        }
    }

    applyPhysics(deltaTime);
    updateAnimation(deltaTime);

    sprite_.setPosition(position_);
}

void Player::handleInput(const sf::Keyboard::Key& key, bool pressed) {
    if (state_ == PlayerState::Dead) return;

    // Jump input
    if (key == sf::Keyboard::Space || key == sf::Keyboard::Up || key == sf::Keyboard::W) {
        if (pressed && !spacePressed_ && !isSliding_) {
            jump();
            spacePressed_ = true;
        }
        else if (!pressed) {
            spacePressed_ = false;
        }
    }

    // Slide input
    if (key == sf::Keyboard::Down || key == sf::Keyboard::S) {
        downPressed_ = pressed;
        if (pressed && grounded_ && !isSliding_) {
            slide();
        }
    }
}

void Player::applyPhysics(float deltaTime) {
    // Apply gravity on Y axis
    if (!grounded_) {
        velocity_.y += gravity_ * deltaTime;
    }
    else {
        velocity_.y = 0.0f;
    }

    // Update position
    position_.x += velocity_.x * deltaTime;
    position_.y += velocity_.y * deltaTime;

    // Update state based on physics
    if (state_ != PlayerState::Dead && state_ != PlayerState::Sliding) {
        if (!grounded_) {
            state_ = PlayerState::Jumping;
        }
        else if (!isSliding_) {
            // When grounded, decide between Idle and Running based on horizontal speed
            if (std::abs(velocity_.x) > 5.0f) {
                state_ = PlayerState::Running;
            }
            else {
                state_ = PlayerState::Idle;
            }
        }
    }
}

void Player::updateAnimation(float deltaTime) {
    auto& rm = ResourceManager::getInstance();
    static float runAnimTime = 0.0f;

    switch (state_) {
    case PlayerState::Idle:
        if (rm.hasTexture("player_idle")) {
            sprite_.setTexture(rm.getTexture("player_idle"));
            auto size = rm.getTexture("player_idle").getSize();
            sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        }
        break;

    case PlayerState::Running:
        runAnimTime += deltaTime;
        // Alternate between walk1 and walk2 for running animation
        if (runAnimTime >= 0.15f) {
            runAnimTime = 0.0f;
        }
        if (rm.hasTexture("player_run") && rm.hasTexture("player_run2")) {
            if (static_cast<int>(runAnimTime * 10) % 2 == 0) {
                sprite_.setTexture(rm.getTexture("player_run"));
                auto size = rm.getTexture("player_run").getSize();
                sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
            }
            else {
                sprite_.setTexture(rm.getTexture("player_run2"));
                auto size = rm.getTexture("player_run2").getSize();
                sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
            }
        }
        else if (rm.hasTexture("player_run")) {
            sprite_.setTexture(rm.getTexture("player_run"));
            auto size = rm.getTexture("player_run").getSize();
            sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        }
        break;

    case PlayerState::Jumping:
        if (rm.hasTexture("player_jump")) {
            sprite_.setTexture(rm.getTexture("player_jump"));
            auto size = rm.getTexture("player_jump").getSize();
            sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        }
        break;

    case PlayerState::Sliding:
        if (rm.hasTexture("player_slide")) {
            sprite_.setTexture(rm.getTexture("player_slide"));
            auto size = rm.getTexture("player_slide").getSize();
            sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        }
        break;

    case PlayerState::Dead:
        if (rm.hasTexture("player_death")) {
            sprite_.setTexture(rm.getTexture("player_death"));
            auto size = rm.getTexture("player_death").getSize();
            sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        }
        break;
    }

    // Flip sprite based on direction
    float scaleX = facingRight_ ? 0.4f : -0.4f;
    sprite_.setScale(scaleX, 0.4f);
    sprite_.setOrigin(0.0f, 0.0f);
}

void Player::jump() {
    if (state_ == PlayerState::Dead) return;

    // Reset jump counter when taking off from the ground
    if (grounded_) {
        jumpsUsed_ = 0;
    }

    if (jumpsUsed_ >= maxJumps_) return;

    ++jumpsUsed_;

    float strength = (jumpsUsed_ == 1) ? smallJumpStrength_ : jumpStrength_;
    velocity_.y = strength;
    grounded_ = false;
    state_ = PlayerState::Jumping;
    jumpAnim_.reset();
    jumpSound_.play();
}

void Player::slide() {
    if (grounded_ && !isSliding_ && state_ != PlayerState::Dead) {
        isSliding_ = true;
        slideTimer_ = slideDuration_;
        state_ = PlayerState::Sliding;
        slideAnim_.reset();
        slideSound_.play();
    }
}

void Player::die() {
    if (state_ != PlayerState::Dead) {
        state_ = PlayerState::Dead;
        velocity_ = sf::Vector2f(0.0f, -200.0f);
        deathAnim_.reset();
        deathSound_.play();
    }
}

void Player::reset() {
    state_ = PlayerState::Idle;
    position_ = sf::Vector2f(100.0f, 400.0f);
    velocity_ = sf::Vector2f(0.0f, 0.0f);
    grounded_ = false;
    isSliding_ = false;
    slideTimer_ = 0.0f;
    facingRight_ = true;
    score_ = 0;
    spacePressed_ = false;
    downPressed_ = false;
    jumpsUsed_ = 0;

    idleAnim_.reset();
    runAnim_.reset();
    jumpAnim_.reset();
    slideAnim_.reset();
    deathAnim_.reset();

    sprite_.setPosition(position_);
}

bool Player::isDead() const {
    return state_ == PlayerState::Dead;
}

bool Player::isGrounded() const {
    return grounded_;
}

void Player::setGrounded(bool grounded) {
    grounded_ = grounded;
    if (grounded_ && state_ == PlayerState::Jumping && !isSliding_) {
        state_ = PlayerState::Running;
        jumpAnim_.reset();
        jumpsUsed_ = 0;
    }
}

void Player::addScore(int points) {
    score_ += points;
}

int Player::getScore() const {
    return score_;
}

void Player::resetScore() {
    score_ = 0;
}

void Player::setScore(int score) {
    score_ = std::max(0, score);
}

void Player::setRunSpeed(float speed) {
    runSpeed_ = speed;
}

float Player::getRunSpeed() const {
    float iceBonus = iceTimer_ > 0.0f ? 0.4f : 1.0f;
    return runSpeed_ * speedBoostMultiplier_ * iceBonus;
}

void Player::applySpeedBoost(float multiplier, float duration) {
    speedBoostMultiplier_ = multiplier;
    speedBoostTimer_ = duration;
}

void Player::setIceTime(float duration) {
    iceTimer_ = std::max(iceTimer_, duration);
}

void Player::bounce(float strength) {
    velocity_.y = strength;
    grounded_ = false;
    state_ = PlayerState::Jumping;
    jumpsUsed_ = 1; // allow one extra jump after bouncing
}

sf::FloatRect Player::getBounds() const {
    auto bounds = sprite_.getGlobalBounds();
    // Slightly shrink the hitbox to better match the visible character and
    // reduce frustrating side collisions.
    float shrinkX = bounds.width * 0.15f;
    float shrinkY = bounds.height * 0.10f;
    bounds.left += shrinkX;
    bounds.width -= 2.0f * shrinkX;
    bounds.top += shrinkY;
    bounds.height -= 2.0f * shrinkY;

    if (isSliding_) {
        bounds.height *= 0.6f; // Make hitbox smaller when sliding
    }
    return bounds;
}

sf::Vector2f Player::getPosition() const {
    return position_;
}

void Player::setPosition(const sf::Vector2f& pos) {
    position_ = pos;
    sprite_.setPosition(position_);
}

void Player::setVelocityY(float vy) {
    velocity_.y = vy;
}

void Player::render(sf::RenderWindow& window) {
    window.draw(sprite_);
}
