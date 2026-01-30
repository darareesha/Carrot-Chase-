/*#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "ResourceManager.h"

enum class CollectibleType {
    Coin,
    Gem,
    Candy,
    Heart,
    Magnet,
    Shield,
    SpeedBoost,
    DoublePoints
};

class Collectible {
public:
    Collectible(const sf::Vector2f& position, CollectibleType type);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    bool isCollected() const;
    void collect();

    CollectibleType getType() const;
    int getPoints() const;

    bool isActive() const;
    void setActive(bool active);
    bool isPowerUp() const;
    void attractTowards(const sf::Vector2f& target, float strength, float deltaTime);
    void setPosition(const sf::Vector2f& position);

private:
    void loadAnimation();
    std::string getTextureName() const;

    sf::Sprite sprite_;
    sf::Vector2f position_;
    CollectibleType type_;
    bool collected_;
    bool active_;

    Animation anim_;
    float floatOffset_;
    float floatTime_;
    sf::Vector2f velocity_;
};
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "Animation.h"
#include "ResourceManager.h"

enum class CollectibleType {
    Coin,
    Gem,
    Candy,
    Heart,
    Magnet,
    Shield,
    SpeedBoost,
    DoublePoints,
    ExtraLife  // ADD THIS LINE
};

class Collectible {
public:
    Collectible(const sf::Vector2f& position, CollectibleType type);

    void update(float deltaTime);
    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;

    bool isCollected() const;
    void collect();

    CollectibleType getType() const;
    int getPoints() const;

    bool isActive() const;
    void setActive(bool active);
    bool isPowerUp() const;
    void attractTowards(const sf::Vector2f& target, float strength, float deltaTime);
    void setPosition(const sf::Vector2f& position);

private:
    void loadAnimation();
    std::string getTextureName() const;

    sf::Sprite sprite_;
    sf::Vector2f position_;
    CollectibleType type_;
    bool collected_;
    bool active_;

    Animation anim_;
    float floatOffset_;
    float floatTime_;
    sf::Vector2f velocity_;
};


