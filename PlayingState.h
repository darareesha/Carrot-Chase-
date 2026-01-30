#pragma once

#include "GameState.h"
#include "Player.h"
#include "Enemy.h"
#include "Collectible.h"
#include "Platform.h"
#include "Background.h"
#include "HUD.h"
#include <vector>
#include <memory>
#include <random>
#include <deque>
#include <string>

class PlayingState : public GameState {
public:
    PlayingState();
    ~PlayingState();

    void handleInput(sf::Event& event) override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    struct ScorePopup {
        sf::Text text;
        sf::Vector2f velocity;
        float lifetime;
    };

    struct Projectile {
        sf::CircleShape shape;
        sf::Vector2f velocity;
        bool active;
    };

    struct Decoration {
        sf::Sprite sprite;
        bool harmful;
    };

    void spawnEnemy();
    void spawnCollectiblePattern(const Platform& platform);
    void checkCollisions(const sf::FloatRect& prevPlayerBounds);
    void updateCamera();
    void ensureGround();
    void ensurePlatforms();
    void cleanupOldEntities();
    void updatePowerUps(float deltaTime);
    void updateDifficulty();
    void handleProjectiles(float deltaTime);
    void triggerScreenShake(float intensity, float duration);
    void updateScorePopups(float deltaTime);
    void addScorePopup(const sf::Vector2f& position, int points);
    int loadHighScore() const;
    void saveHighScore() const;

    Player player_;
    Background background_;
    HUD hud_;

    std::vector<std::unique_ptr<Enemy>> enemies_;
    std::vector<std::unique_ptr<Collectible>> collectibles_;
    std::vector<std::unique_ptr<Platform>> platforms_;
    std::vector<Projectile> projectiles_;
    std::deque<ScorePopup> scorePopups_;
    std::vector<Decoration> decorations_;
    std::vector<sf::Sprite> clouds_;
    std::vector<float> cloudSpeeds_;

    int lives_;

    float cameraX_;
    float cameraSmoothX_;
    sf::Vector2f cameraShakeOffset_;
    float cameraShakeTime_;
    float cameraShakeIntensity_;
    float generationX_;
    float groundGenerationX_;
    float comboTimer_;
    int comboCount_;
    float scoreMultiplier_;
    float doublePointsTimer_;
    float magnetTimer_;
    float shieldTimer_;
    float magnetRadius_;
    int highScore_;
    float distance_;
    float platformMinGap_;
    float platformMaxGap_;
    float difficultyTimer_;
    float enemySpawnTimer_;
    float projectileSpeed_;
    float playerBaseSpeed_;
    float lastEnemyBatchDistance_;
    float lastGoldenCarrotDistance_;
    float lastCoinDistance_;

    bool paused_;
    bool magnetActive_;
    bool shieldActive_;
    bool doublePointsActive_;

    float worldTime_;

    std::mt19937 rng_;
    sf::Sound collectSound_;
    sf::Sound enemyHitSound_;
    sf::Sound hitSound_;
    sf::Sound ouchSound_;

    sf::RectangleShape pauseOverlay_;
    sf::Text pauseText_;
    sf::Font* font_;
};
