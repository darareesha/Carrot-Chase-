#include "Enemy.h"
#include <cmath>

Enemy::Enemy(const sf::Vector2f& position, EnemyType type, float speed)
    : position_(position)
    , target_(position)
    , speed_(speed)
    , active_(true)
    , facingRight_(false)
    , type_(type)
    , sineTimer_(0.0f)
    , shootTimer_(0.0f)
    , wantsToShoot_(false)
    , animTimer_(0.0f)
    , currentFrame_(0) {

    loadAnimation();

    if (!frames_.empty()) {
        sprite_.setTexture(*frames_[0]);
    }
    sprite_.setPosition(position_);
    sprite_.setScale(0.7f, 0.7f);
}

void Enemy::loadAnimation() {
    auto& rm = ResourceManager::getInstance();
    auto pushFrame = [&](const std::string& name) {
        if (rm.hasTexture(name)) {
            frames_.push_back(&rm.getTexture(name));
        }
        };

    switch (type_) {
    case EnemyType::Flyer:
        pushFrame("flyMan_fly");
        pushFrame("flyMan_still_fly");
        break;
    case EnemyType::Shooter:
        pushFrame("wingMan1");
        pushFrame("wingMan2");
        break;
    case EnemyType::Chaser:
        pushFrame("spikeMan_jump");
        pushFrame("spikeMan_walk1");
        break;
    case EnemyType::Walker:
    default:
        pushFrame("spikeMan_walk1");
        pushFrame("spikeMan_walk2");
        break;
    }

    if (frames_.empty() && rm.hasTexture("enemy")) {
        frames_.push_back(&rm.getTexture("enemy"));
    }
}

void Enemy::update(float deltaTime) {
    if (!active_) return;
    wantsToShoot_ = false;

    updateMovement(deltaTime);
    updateAnimation(deltaTime);

    // Deactivate if off screen
    if (position_.x < -200.0f) {
        active_ = false;
    }
}

void Enemy::updateMovement(float deltaTime) {
    switch (type_) {
    case EnemyType::Flyer: {
        sineTimer_ += deltaTime * 3.0f;
        position_.x -= speed_ * deltaTime;
        position_.y += std::sin(sineTimer_) * 40.0f * deltaTime;
        break;
    }
    case EnemyType::Shooter: {
        shootTimer_ += deltaTime;
        position_.x -= (speed_ * 0.6f) * deltaTime;
        if (shootTimer_ >= 2.5f) {
            wantsToShoot_ = true;
            shootTimer_ = 0.0f;
        }
        break;
    }
    case EnemyType::Chaser: {
        float acceleration = 1.0f + std::max(0.0f, 400.0f - std::abs(target_.x - position_.x)) / 400.0f;
        position_.x -= speed_ * acceleration * deltaTime;
        break;
    }
    case EnemyType::Walker:
    default:
        position_.x -= speed_ * deltaTime;
        break;
    }

    sprite_.setPosition(position_);
    sprite_.setScale(0.7f, 0.7f);
    sprite_.setOrigin(0.0f, 0.0f);
}

void Enemy::updateAnimation(float deltaTime) {
    if (frames_.empty()) return;

    animTimer_ += deltaTime;
    if (animTimer_ >= 0.15f) {
        animTimer_ = 0.0f;
        currentFrame_ = (currentFrame_ + 1) % frames_.size();
    }
    sprite_.setTexture(*frames_[currentFrame_]);
}

void Enemy::render(sf::RenderWindow& window) {
    if (active_) {
        window.draw(sprite_);
    }
}

sf::FloatRect Enemy::getBounds() const {
    return sprite_.getGlobalBounds();
}

sf::Vector2f Enemy::getPosition() const {
    return position_;
}

bool Enemy::isActive() const {
    return active_;
}

void Enemy::setActive(bool active) {
    active_ = active;
}

void Enemy::reset(const sf::Vector2f& position) {
    position_ = position;
    active_ = true;
    sineTimer_ = 0.0f;
    shootTimer_ = 0.0f;
    animTimer_ = 0.0f;
    currentFrame_ = 0;
    sprite_.setPosition(position_);
}

void Enemy::setTargetPosition(const sf::Vector2f& target) {
    target_ = target;
}

bool Enemy::wantsToShoot() const {
    return wantsToShoot_;
}

sf::Vector2f Enemy::getShootOrigin() const {
    return position_ + sf::Vector2f(-20.0f, 10.0f);
}

