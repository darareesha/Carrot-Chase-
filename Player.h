#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include "ResourceManager.h"

enum class PlayerState {
    Idle,
    Running,
    Jumping,
    Sliding,
    Dead
};

class Player {
public:
    Player();

    void update(float deltaTime);
    void handleInput(const sf::Keyboard::Key& key, bool pressed);
    void render(sf::RenderWindow& window);

    sf::FloatRect getBounds() const;
    sf::Vector2f getPosition() const;
    void setPosition(const sf::Vector2f& pos);

    void jump();
    void slide();
    void die();
    void reset();

    bool isDead() const;
    bool isGrounded() const;
    void setGrounded(bool grounded);

    void addScore(int points);
    int getScore() const;
    void resetScore();
    void setScore(int score);

    void setRunSpeed(float speed);
    float getRunSpeed() const;
    void applySpeedBoost(float multiplier, float duration);
    void setIceTime(float duration);
    void bounce(float strength);
    void setVelocityY(float vy);

    PlayerState getState() const { return state_; }

private:
    void loadAnimations();
    void updateAnimation(float deltaTime);
    void applyPhysics(float deltaTime);

    sf::Sprite sprite_;
    PlayerState state_;

    // Animations
    Animation idleAnim_;
    Animation runAnim_;
    Animation jumpAnim_;
    Animation slideAnim_;
    Animation deathAnim_;

    // Physics
    sf::Vector2f velocity_;
    sf::Vector2f position_;
    float gravity_;
    float jumpStrength_;
    float smallJumpStrength_;
    float runSpeed_;
    float speedBoostMultiplier_;
    float speedBoostTimer_;
    float iceTimer_;
    float slideDuration_;
    float slideTimer_;

    bool grounded_;
    bool facingRight_;
    bool isSliding_;

    int score_;

    // Sound effects
    sf::Sound jumpSound_;
    sf::Sound slideSound_;
    sf::Sound deathSound_;

    // Input state
    bool spacePressed_;
    bool downPressed_;
    bool moveLeftHeld_;
    bool moveRightHeld_;

    // Jump state
    int maxJumps_;
    int jumpsUsed_;
};