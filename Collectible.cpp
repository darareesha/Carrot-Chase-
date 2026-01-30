/*
#include "Collectible.h"
#include <cmath>

Collectible::Collectible(const sf::Vector2f& position, CollectibleType type)
    : position_(position)
    , type_(type)
    , collected_(false)
    , active_(true)
    , floatOffset_(0.0f)
    , floatTime_(0.0f)
    , velocity_(0.0f, 0.0f) {

    loadAnimation();

    auto& rm = ResourceManager::getInstance();
    std::string texName = getTextureName();
    if (rm.hasTexture(texName)) {
        sprite_.setTexture(rm.getTexture(texName));
        auto size = rm.getTexture(texName).getSize();
        sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }
    sprite_.setPosition(position_);
    // Make carrots and small collectibles feel tiny & cute, powerups a bit larger
    float scale = 0.6f;
    if (isPowerUp()) {
        scale = 0.85f;
    }
    sprite_.setScale(scale, scale);
}

void Collectible::loadAnimation() {
    auto& rm = ResourceManager::getInstance();
    std::string texName = getTextureName();

    if (rm.hasTexture(texName)) {
        auto& tex = rm.getTexture(texName);
        auto size = tex.getSize();
        anim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        anim_.setFrameTime(0.15f);
    }
}

std::string Collectible::getTextureName() const {
    switch (type_) {
    case CollectibleType::Coin:        // basic carrot
        return "carrot";
    case CollectibleType::Gem:         // golden carrot
        return "carrot_gold";
    case CollectibleType::Candy:
        return "mushroom_red";
    case CollectibleType::Heart:
        return "coin_gold";
    case CollectibleType::Magnet:
        return "powerup_bunny";
    case CollectibleType::Shield:
        return "powerup_bubble";
    case CollectibleType::SpeedBoost:
        return "powerup_jetpack";
    case CollectibleType::DoublePoints:
        return "powerup_wings";
    default:
        return "carrot";
    }
}

void Collectible::update(float deltaTime) {
    if (!active_ || collected_) return;

    // Floating animation
    floatTime_ += deltaTime * 2.0f;
    floatOffset_ = std::sin(floatTime_) * 10.0f;

    // Update animation
    anim_.update(deltaTime);

    // Apply velocity (magnet effect)
    position_ += velocity_ * deltaTime;
    velocity_ *= 0.9f;

    // Update sprite position with floating effect
    sf::Vector2f renderPos = position_;
    renderPos.y += floatOffset_;
    sprite_.setPosition(renderPos);
    sprite_.setTextureRect(anim_.getCurrentFrame());
}

void Collectible::render(sf::RenderWindow& window) {
    if (active_ && !collected_) {
        window.draw(sprite_);
    }
}

sf::FloatRect Collectible::getBounds() const {
    return sprite_.getGlobalBounds();
}

sf::Vector2f Collectible::getPosition() const {
    return position_;
}

bool Collectible::isCollected() const {
    return collected_;
}

void Collectible::collect() {
    collected_ = true;
    active_ = false;
}

CollectibleType Collectible::getType() const {
    return type_;
}

int Collectible::getPoints() const {
    switch (type_) {
    case CollectibleType::Coin:  // carrot
        return 10;
    case CollectibleType::Gem:   // golden carrot = higher value
        return 25;
    case CollectibleType::Candy:
        return 5;
    case CollectibleType::Heart:
        return 10;
    default:
        return 0;
    }
}

bool Collectible::isActive() const {
    return active_ && !collected_;
}

void Collectible::setActive(bool active) {
    active_ = active;
}

bool Collectible::isPowerUp() const {
    return type_ == CollectibleType::Magnet ||
        type_ == CollectibleType::Shield ||
        type_ == CollectibleType::SpeedBoost ||
        type_ == CollectibleType::DoublePoints;
}

void Collectible::attractTowards(const sf::Vector2f& target, float strength, float deltaTime) {
    sf::Vector2f direction = target - position_;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 1.0f) {
        direction /= length;
        velocity_ += direction * strength * deltaTime;
    }
}

void Collectible::setPosition(const sf::Vector2f& pos) {
    position_ = pos;
    sprite_.setPosition(position_);
}
*/

#include "Collectible.h"
#include <cmath>

Collectible::Collectible(const sf::Vector2f& position, CollectibleType type)
    : position_(position)
    , type_(type)
    , collected_(false)
    , active_(true)
    , floatOffset_(0.0f)
    , floatTime_(0.0f)
    , velocity_(0.0f, 0.0f) {

    loadAnimation();

    auto& rm = ResourceManager::getInstance();
    std::string texName = getTextureName();
    if (rm.hasTexture(texName)) {
        sprite_.setTexture(rm.getTexture(texName));
        auto size = rm.getTexture(texName).getSize();
        sprite_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
    }
    sprite_.setPosition(position_);

    // Adjust scale based on type
    float scale = 0.6f;
    if (isPowerUp()) {
        scale = 0.85f;
        // ExtraLife could have a different scale if desired
        if (type_ == CollectibleType::ExtraLife) {
            scale = 0.7f;
        }
    }
    sprite_.setScale(scale, scale);
}

void Collectible::loadAnimation() {
    auto& rm = ResourceManager::getInstance();
    std::string texName = getTextureName();

    if (rm.hasTexture(texName)) {
        auto& tex = rm.getTexture(texName);
        auto size = tex.getSize();
        anim_.addFrame(sf::IntRect(0, 0, static_cast<int>(size.x), static_cast<int>(size.y)));
        anim_.setFrameTime(0.15f);
    }
}

std::string Collectible::getTextureName() const {
    switch (type_) {
    case CollectibleType::Coin:        // basic carrot
        return "carrot";
    case CollectibleType::Gem:         // golden carrot
        return "carrot_gold";
    case CollectibleType::Candy:
        return "mushroom_red";
    case CollectibleType::Heart:
        return "coin_gold";
    case CollectibleType::Magnet:
        return "powerup_bunny";
    case CollectibleType::Shield:
        return "powerup_bubble";
    case CollectibleType::SpeedBoost:
        return "powerup_jetpack";
    case CollectibleType::DoublePoints:
        return "powerup_wings";
    case CollectibleType::ExtraLife:    
        return "lifes";
    default:
        return "carrot";
    }
}

void Collectible::update(float deltaTime) {
    if (!active_ || collected_) return;

    // Floating animation
    floatTime_ += deltaTime * 2.0f;
    floatOffset_ = std::sin(floatTime_) * 10.0f;

    // Update animation
    anim_.update(deltaTime);

    // Apply velocity (magnet effect)
    position_ += velocity_ * deltaTime;
    velocity_ *= 0.9f;

    // Update sprite position with floating effect
    sf::Vector2f renderPos = position_;
    renderPos.y += floatOffset_;
    sprite_.setPosition(renderPos);
    sprite_.setTextureRect(anim_.getCurrentFrame());
}

void Collectible::render(sf::RenderWindow& window) {
    if (active_ && !collected_) {
        window.draw(sprite_);
    }
}

sf::FloatRect Collectible::getBounds() const {
    return sprite_.getGlobalBounds();
}

sf::Vector2f Collectible::getPosition() const {
    return position_;
}

bool Collectible::isCollected() const {
    return collected_;
}

void Collectible::collect() {
    collected_ = true;
    active_ = false;
}

CollectibleType Collectible::getType() const {
    return type_;
}

int Collectible::getPoints() const {
    switch (type_) {
    case CollectibleType::Coin:  // carrot
        return 10;
    case CollectibleType::Gem:   // golden carrot = higher value
        return 25;
    case CollectibleType::Candy:
        return 5;
    case CollectibleType::Heart:
        return 10;
    case CollectibleType::ExtraLife:  // ADD THIS CASE
        return 0;
    default:
        return 0;
    }
}

bool Collectible::isActive() const {
    return active_ && !collected_;
}

void Collectible::setActive(bool active) {
    active_ = active;
}

bool Collectible::isPowerUp() const {
    return type_ == CollectibleType::Magnet ||
        type_ == CollectibleType::Shield ||
        type_ == CollectibleType::SpeedBoost ||
        type_ == CollectibleType::DoublePoints ||
        type_ == CollectibleType::ExtraLife; 
}

void Collectible::attractTowards(const sf::Vector2f& target, float strength, float deltaTime) {
    sf::Vector2f direction = target - position_;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    if (length > 1.0f) {
        direction /= length;
        velocity_ += direction * strength * deltaTime;
    }
}

void Collectible::setPosition(const sf::Vector2f& pos) {
    position_ = pos;
    sprite_.setPosition(position_);
}