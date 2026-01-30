#include "Platform.h"
#include "Player.h"

namespace {
    sf::Color getPlatformColor(PlatformType type) {
        switch (type) {
        case PlatformType::MovingHorizontal: return sf::Color(173, 216, 230); // light blue
        case PlatformType::MovingVertical:   return sf::Color(255, 182, 193); // pink
        case PlatformType::Breaking:         return sf::Color(255, 160, 122); // salmon
        case PlatformType::Bouncy:           return sf::Color(255, 255, 102); // yellow
        case PlatformType::Ice:              return sf::Color(200, 255, 255);
        default:                             return sf::Color(144, 238, 144);
        }
    }
}

Platform::Platform(const sf::Vector2f& position,
    const sf::Vector2f& size,
    PlatformType type,
    float movementRange,
    float movementSpeed)
    : position_(position)
    , size_(size)
    , type_(type)
    , originPosition_(position)
    , movementRange_(movementRange)
    , movementSpeed_(movementSpeed)
    , movementTimer_(0.0f)
    , movingForward_(true)
    , breaking_(false)
    , breakTimer_(0.0f)
    , active_(true) {

    auto& rm = ResourceManager::getInstance();

    shape_.setPosition(position_);
    shape_.setSize(size_);

    const std::string textureName =
        type == PlatformType::Ice ? "platform_snow" :
        type == PlatformType::Bouncy ? "platform_cake" :
        type == PlatformType::MovingHorizontal || type == PlatformType::MovingVertical ? "platform_sand" :
        "platform";

    if (rm.hasTexture(textureName)) {
        shape_.setTexture(&rm.getTexture(textureName));
        shape_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size_.x), static_cast<int>(size_.y)));
    }
    else if (rm.hasTexture("platform")) {
        shape_.setTexture(&rm.getTexture("platform"));
        shape_.setTextureRect(sf::IntRect(0, 0, static_cast<int>(size_.x), static_cast<int>(size_.y)));
    }
    else {
        shape_.setFillColor(getPlatformColor(type));
        shape_.setOutlineColor(sf::Color(34, 139, 34));
        shape_.setOutlineThickness(2.0f);
    }
}

void Platform::update(float deltaTime) {
    if (!active_) return;
    updateMovement(deltaTime);
    updateBreaking(deltaTime);
}

void Platform::updateMovement(float deltaTime) {
    if (type_ == PlatformType::MovingHorizontal) {
        float displacement = movementSpeed_ * deltaTime * (movingForward_ ? 1.0f : -1.0f);
        position_.x += displacement;
        if (std::abs(position_.x - originPosition_.x) >= movementRange_) {
            movingForward_ = !movingForward_;
        }
        shape_.setPosition(position_);
    }
    else if (type_ == PlatformType::MovingVertical) {
        float displacement = movementSpeed_ * deltaTime * (movingForward_ ? 1.0f : -1.0f);
        position_.y += displacement;
        if (std::abs(position_.y - originPosition_.y) >= movementRange_) {
            movingForward_ = !movingForward_;
        }
        shape_.setPosition(position_);
    }
}

void Platform::startBreaking() {
    if (type_ == PlatformType::Breaking && !breaking_) {
        breaking_ = true;
        breakTimer_ = 1.0f; // seconds before disappearing
        shape_.setFillColor(sf::Color(255, 120, 120));
    }
}

void Platform::updateBreaking(float deltaTime) {
    if (!breaking_) return;
    breakTimer_ -= deltaTime;
    if (breakTimer_ <= 0.0f) {
        active_ = false;
    }
    else {
        float alpha = std::max(0.0f, breakTimer_);
        shape_.setFillColor(sf::Color(255, 120, 120, static_cast<sf::Uint8>(alpha / 1.0f * 255)));
    }
}

bool Platform::isBroken() const {
    return !active_;
}

void Platform::render(sf::RenderWindow& window) {
    if (active_) {
        window.draw(shape_);
    }
}

sf::FloatRect Platform::getBounds() const {
    return shape_.getGlobalBounds();
}

sf::Vector2f Platform::getPosition() const {
    return position_;
}

void Platform::applyTypeEffect(Player& player) {
    switch (type_) {
    case PlatformType::Bouncy:
        player.bounce(-900.0f);
        break;
    case PlatformType::Ice:
        player.setIceTime(0.8f);
        break;
    case PlatformType::Breaking:
        startBreaking();
        break;
    default:
        break;
    }
}