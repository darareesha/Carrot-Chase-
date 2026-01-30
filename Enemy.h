#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.h"
#include <vector>
#include "ResourceManager.h"

enum class EnemyType {
    Walker,
    Flyer,
    Shooter,
    Chaser
};

class Enemy {
public:
    Enemy(const sf::Vector2f& position, EnemyType type = EnemyType::Walker, float speed = 150.0f);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);
    void setTargetPosition(const sf::Vector2f& target);
    bool wantsToShoot() const;
    sf::Vector2f getShootOrigin() const;

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    EnemyType getType() const { return type_; }

    bool isActive() const;
    void setActive(bool active);

    void reset(const sf::Vector2f& position);

private:
    void loadAnimation();
    void updateAnimation(float deltaTime);
    void updateMovement(float deltaTime);

    sf::Sprite sprite_;
    sf::Vector2f position_;
    sf::Vector2f target_;
    float speed_;
    bool active_;
    bool facingRight_;
    EnemyType type_;
    float sineTimer_;
    float shootTimer_;
    bool wantsToShoot_;

    Animation walkAnim_;
    std::vector<sf::Texture*> frames_;
    float animTimer_;
    std::size_t currentFrame_;
};