#pragma once

#include <SFML/Graphics.hpp>
#include "ResourceManager.h"
#include <string>

class Player;

enum class PlatformType {
    Normal,
    MovingHorizontal,
    MovingVertical,
    Breaking,
    Bouncy,
    Ice
};

class Platform {
public:
    Platform(const sf::Vector2f& position,
        const sf::Vector2f& size,
        PlatformType type = PlatformType::Normal,
        float movementRange = 0.0f,
        float movementSpeed = 0.0f);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void startBreaking();
    bool isBroken() const;
    PlatformType getType() const { return type_; }

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const { return size_; }
    void applyTypeEffect(Player& player);
    bool isActive() const { return active_; }
    void deactivate() { active_ = false; }

private:
    void updateMovement(float deltaTime);
    void updateBreaking(float deltaTime);

    sf::RectangleShape shape_;
    sf::Vector2f position_;
    sf::Vector2f size_;
    PlatformType type_;
    sf::Vector2f originPosition_;
    float movementRange_;
    float movementSpeed_;
    float movementTimer_;
    bool movingForward_;

    bool breaking_;
    float breakTimer_;
    bool active_;
};



