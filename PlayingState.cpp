#include "PlayingState.h"
#include "Game.h"
#include "PauseState.h"
#include "GameOverState.h"
#include "ResourceManager.h"
#include <algorithm>
#include <cmath>
#include <fstream>
#include <sstream>

namespace {
    float randomFloat(std::mt19937& rng, float minValue, float maxValue) {
        std::uniform_real_distribution<float> dist(minValue, maxValue);
        return dist(rng);
    }

    int randomInt(std::mt19937& rng, int minValue, int maxValue) {
        std::uniform_int_distribution<int> dist(minValue, maxValue);
        return dist(rng);
    }
}

PlayingState::PlayingState()
    : cameraX_(0.0f)
    , cameraSmoothX_(0.0f)
    , cameraShakeOffset_(0.0f, 0.0f)
    , cameraShakeTime_(0.0f)
    , cameraShakeIntensity_(0.0f)
    , generationX_(800.0f)
    , groundGenerationX_(0.0f)
    , comboTimer_(0.0f)
    , comboCount_(0)
    , scoreMultiplier_(1.0f)
    , doublePointsTimer_(0.0f)
    , magnetTimer_(0.0f)
    , shieldTimer_(0.0f)
    , magnetRadius_(220.0f)
    , highScore_(0)
    , distance_(0.0f)
    , platformMinGap_(120.0f)
    , platformMaxGap_(260.0f)
    , difficultyTimer_(0.0f)
    , enemySpawnTimer_(0.0f)
    , projectileSpeed_(420.0f)
    , playerBaseSpeed_(240.0f)
    , lastEnemyBatchDistance_(0.0f)
    , lastGoldenCarrotDistance_(0.0f)
    , lastCoinDistance_(0.0f)
    , paused_(false)
    , magnetActive_(false)
    , shieldActive_(false)
    , doublePointsActive_(false)
    , worldTime_(0.0f)
    , rng_(std::random_device{}())
    , font_(nullptr)
    , lives_(3) {

    auto& rm = ResourceManager::getInstance();

    if (rm.hasFont("default")) {
        font_ = &rm.getFont("default");
    }

    pauseOverlay_.setSize(sf::Vector2f(1280.0f, 720.0f));
    pauseOverlay_.setFillColor(sf::Color(0, 0, 0, 160));

    if (font_) {
        pauseText_.setFont(*font_);
        pauseText_.setCharacterSize(60);
        pauseText_.setFillColor(sf::Color::White);
        pauseText_.setStyle(sf::Text::Bold);
        pauseText_.setString("PAUSED\nPress P to Resume");
        pauseText_.setPosition(640.0f - pauseText_.getGlobalBounds().width / 2.0f,
            360.0f - pauseText_.getGlobalBounds().height / 2.0f);
    }

    if (rm.hasSoundBuffer("collect")) {
        collectSound_.setBuffer(rm.getSoundBuffer("collect"));
        collectSound_.setVolume(55.0f);
    }
    if (rm.hasSoundBuffer("enemy_hit")) {
        enemyHitSound_.setBuffer(rm.getSoundBuffer("enemy_hit"));
        enemyHitSound_.setVolume(48.0f);
    }
    if (rm.hasSoundBuffer("hit")) {
        hitSound_.setBuffer(rm.getSoundBuffer("hit"));
        hitSound_.setVolume(55.0f);
    }
    if (rm.hasSoundBuffer("ouch")) {
        ouchSound_.setBuffer(rm.getSoundBuffer("ouch"));
        ouchSound_.setVolume(55.0f);
    }

    player_.setRunSpeed(playerBaseSpeed_);
    highScore_ = loadHighScore();

    ensureGround();
    ensurePlatforms();

    // Setup clouds in the sky
    auto& rmCloud = ResourceManager::getInstance();
    if (rmCloud.hasTexture("cloud")) {
        for (int i = 0; i < 5; ++i) {
            sf::Sprite cloud(rmCloud.getTexture("cloud"));
            float scale = randomFloat(rng_, 0.6f, 1.0f);
            cloud.setScale(scale, scale);
            float x = randomFloat(rng_, 0.0f, 1600.0f);
            float y = randomFloat(rng_, 60.0f, 220.0f);
            cloud.setPosition(x, y);
            clouds_.push_back(cloud);
            cloudSpeeds_.push_back(randomFloat(rng_, 20.0f, 40.0f));
        }
    }
}

PlayingState::~PlayingState() = default;

void PlayingState::handleInput(sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::P || event.key.code == sf::Keyboard::Escape) {
            paused_ = !paused_;
            if (paused_ && game_) {
                game_->pushState(std::make_unique<PauseState>());
            }
        }
        else if (!paused_) {
            player_.handleInput(event.key.code, true);
        }
    }
    else if (event.type == sf::Event::KeyReleased && !paused_) {
        player_.handleInput(event.key.code, false);
    }
}

void PlayingState::update(float deltaTime) {
    if (paused_) return;

    worldTime_ += deltaTime;
    // Remember previous player bounds for more accurate collision resolution
    sf::FloatRect prevPlayerBounds = player_.getBounds();
    player_.update(deltaTime);
    // Distance should only ever increase, even if the player walks backwards.
    float currentDistance = std::max(0.0f, (player_.getPosition().x - 100.0f) / 50.0f);
    distance_ = std::max(distance_, currentDistance);

    ensureGround();
    ensurePlatforms();
    cleanupOldEntities();

    updateDifficulty();
    updatePowerUps(deltaTime);
    updateCamera();
    background_.update(deltaTime, cameraX_);

    // Distance-based spawns for more predictable pacing
    // 1) Enemies: at most ONE enemy on screen, spawned every ~70m
    if (distance_ - lastEnemyBatchDistance_ >= 70.0f) {
        spawnEnemy();
        lastEnemyBatchDistance_ = distance_;
    }

    // 2) Golden carrot (x2) every 50m on ground
    if (distance_ - lastGoldenCarrotDistance_ >= 50.0f) {
        float x = player_.getPosition().x + 450.0f;
        float y = 570.0f; // on ground surface
        collectibles_.push_back(std::make_unique<Collectible>(sf::Vector2f(x, y), CollectibleType::Gem));
        lastGoldenCarrotDistance_ = distance_;
    }

    // 3) Coin line every ~30m on ground
    if (distance_ - lastCoinDistance_ >= 30.0f) {
        float startX = player_.getPosition().x + 400.0f;
        float y = 570.0f; // on ground surface
        int count = 2;
        for (int i = 0; i < count; ++i) {
            float x = startX + static_cast<float>(i) * 40.0f;
            collectibles_.push_back(std::make_unique<Collectible>(sf::Vector2f(x, y), CollectibleType::Heart)); // using coin_gold as coin
        }
        lastCoinDistance_ = distance_;
    }

    for (auto& platform : platforms_) {
        platform->update(deltaTime);
    }

    for (auto& enemy : enemies_) {
        enemy->setTargetPosition(player_.getPosition());
        enemy->update(deltaTime);
        if (enemy->wantsToShoot()) {
            Projectile projectile;
            projectile.shape.setRadius(10.0f);
            projectile.shape.setOrigin(10.0f, 10.0f);
            projectile.shape.setFillColor(sf::Color(255, 140, 140));
            projectile.shape.setPosition(enemy->getShootOrigin());
            projectile.velocity = sf::Vector2f(-projectileSpeed_, 0.0f);
            projectile.active = true;
            projectiles_.push_back(projectile);
        }
    }

    // Update clouds to drift slowly across the sky
    for (std::size_t i = 0; i < clouds_.size(); ++i) {
        clouds_[i].move(-cloudSpeeds_[i] * deltaTime, 0.0f);
        if (clouds_[i].getPosition().x < cameraX_ - 300.0f) {
            float newX = cameraX_ + 1500.0f;
            float newY = randomFloat(rng_, 60.0f, 220.0f);
            clouds_[i].setPosition(newX, newY);
        }
    }

    for (auto& collectible : collectibles_) {
        if (magnetActive_) {
            float dx = collectible->getPosition().x - player_.getPosition().x;
            if (std::abs(dx) < magnetRadius_) {
                collectible->attractTowards(player_.getPosition(), 700.0f, deltaTime);
            }
        }
        collectible->update(deltaTime);
    }

    handleProjectiles(deltaTime);
    updateScorePopups(deltaTime);
    checkCollisions(prevPlayerBounds);

    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
        [](const std::unique_ptr<Enemy>& e) { return !e->isActive(); }),
        enemies_.end());

    collectibles_.erase(std::remove_if(collectibles_.begin(), collectibles_.end(),
        [](const std::unique_ptr<Collectible>& c) { return !c->isActive(); }),
        collectibles_.end());

    if (player_.isDead()) {
        if (player_.getScore() > highScore_) {
            highScore_ = player_.getScore();
            saveHighScore();
        }
        if (game_) {
            game_->changeState(std::make_unique<GameOverState>(player_.getScore(), highScore_, distance_));
        }
        return;
    }

    const float comboMultiplier = 1.0f + static_cast<float>(comboCount_) * 0.1f;
    hud_.update(player_.getScore(),
        highScore_,
        distance_,
        comboCount_,
        doublePointsActive_ ? comboMultiplier * 2.0f : comboMultiplier,
        shieldActive_,
        magnetActive_,
        doublePointsActive_,
        lives_);
}

void PlayingState::render(sf::RenderWindow& window) {
    sf::View view = window.getView();
    view.setCenter(cameraX_ + 640.0f + cameraShakeOffset_.x, 360.0f + cameraShakeOffset_.y);
    window.setView(view);

    background_.render(window);

    // Parallax clouds in front of distant background
    for (const auto& cloud : clouds_) {
        window.draw(cloud);
    }

    for (const auto& platform : platforms_) {
        platform->render(window);
    }

    // Ground decorations (cactus, mushrooms)
    for (const auto& deco : decorations_) {
        window.draw(deco.sprite);
    }

    for (const auto& collectible : collectibles_) {
        collectible->render(window);
    }

    for (const auto& enemy : enemies_) {
        enemy->render(window);
    }

    for (const auto& projectile : projectiles_) {
        window.draw(projectile.shape);
    }

    player_.render(window);

    if (shieldActive_) {
        sf::CircleShape aura(48.0f);
        aura.setOrigin(48.0f, 48.0f);
        aura.setFillColor(sf::Color(173, 216, 230, 80));
        aura.setOutlineColor(sf::Color(135, 206, 250));
        aura.setOutlineThickness(3.0f);
        aura.setPosition(player_.getPosition().x + 32.0f, player_.getPosition().y + 32.0f);
        window.draw(aura);
    }

    for (const auto& popup : scorePopups_) {
        window.draw(popup.text);
    }

    view.setCenter(640.0f, 360.0f);
    window.setView(view);
    hud_.render(window);

    // Gameplay uses normal OS cursor
    window.setMouseCursorVisible(true);

    if (paused_) {
        window.draw(pauseOverlay_);
        if (font_) {
            window.draw(pauseText_);
        }
    }
}

void PlayingState::spawnEnemy() {
    // Limit total enemies on screen so it doesn't feel overwhelming
    const std::size_t maxEnemiesOnScreen = 1;
    if (enemies_.size() >= maxEnemiesOnScreen) return;

    float difficulty = 1.0f + player_.getScore() / 900.0f;

    // Randomly choose between a ground walker and a flying enemy in the sky
    // After 1 minute, start spawning flyers (copters)
    bool spawnFlyer = (worldTime_ >= 60.0f);

    EnemyType type = spawnFlyer ? EnemyType::Flyer : EnemyType::Walker;

    float x = cameraX_ + 900.0f + randomFloat(rng_, 0.0f, 250.0f);
    float y = spawnFlyer
        ? randomFloat(rng_, 260.0f, 360.0f)   // flyMan in mid-sky
        : 540.0f;                             // ground runner

    // Enemies run slightly faster than player but not too fast
    float base = player_.getRunSpeed() * 1.05f;
    float speed = base * (0.9f + 0.2f * difficulty);

    auto enemy = std::make_unique<Enemy>(sf::Vector2f(x, y), type, speed);
    enemy->setTargetPosition(player_.getPosition());
    enemies_.push_back(std::move(enemy));
}

void PlayingState::spawnCollectiblePattern(const Platform& platform) {
    if (platform.getSize().x < 120.0f) return;

    float topY = platform.getPosition().y - 18.0f;
    int count = std::clamp(static_cast<int>(platform.getSize().x / 60.0f), 2, 7);
    float startX = platform.getPosition().x + 20.0f;
    float step = (platform.getSize().x - 40.0f) / std::max(1, count - 1);
    float roll = randomFloat(rng_, 0.0f, 1.0f);

    auto placeCollectible = [&](const sf::Vector2f& pos, CollectibleType type) {
        collectibles_.push_back(std::make_unique<Collectible>(pos, type));
        };

    if (roll < 0.5f) {
        // Simple, readable horizontal line of carrots
        for (int i = 0; i < count; ++i) {
            placeCollectible({ startX + step * i, topY }, CollectibleType::Coin);
        }
    }
    else if (roll < 0.85f) {
        // Gentle zig-zag carrots across the platform
        for (int i = 0; i < count; ++i) {
            float zigzag = (i % 2 == 0) ? -18.0f : 12.0f;
            placeCollectible({ startX + step * i, topY + zigzag }, CollectibleType::Coin);
        }
    }
    else {
        // Small cluster of golden carrots near the far end for higher risk/reward
        int gems = std::min(3, count);
        float baseX = startX + step * (count - 1) - 30.0f;
        for (int i = 0; i < gems; ++i) {
            float offsetX = randomFloat(rng_, -10.0f, 20.0f);
            float offsetY = -randomFloat(rng_, 0.0f, 16.0f);
            placeCollectible({ baseX + offsetX, topY + offsetY }, CollectibleType::Gem);
        }
    }

    if (randomFloat(rng_, 0.0f, 1.0f) < 0.18f) {
        CollectibleType powerType = CollectibleType::Magnet;
        float powerRoll = randomFloat(rng_, 0.0f, 1.0f);
        if (powerRoll < 0.25f) powerType = CollectibleType::Magnet;
        else if (powerRoll < 0.5f) powerType = CollectibleType::Shield;
        else if (powerRoll < 0.75f) powerType = CollectibleType::SpeedBoost;
        else powerType = CollectibleType::DoublePoints;

        placeCollectible({ platform.getPosition().x + platform.getSize().x / 2.0f, topY - 24.0f }, powerType);
    }
}

void PlayingState::checkCollisions(const sf::FloatRect& prevPlayerBounds) {
    sf::FloatRect playerBounds = player_.getBounds();
    bool grounded = false;

    for (auto& platform : platforms_) {
        if (!platform->isActive()) continue;
        sf::FloatRect platformBounds = platform->getBounds();
        if (playerBounds.intersects(platformBounds)) {
            float prevBottom = prevPlayerBounds.top + prevPlayerBounds.height;
            float platformTop = platformBounds.top;
            float currBottom = playerBounds.top + playerBounds.height;

            // Only treat as a landing if the player was above the platform last frame
            // and is now crossing its top from above (prevents "sticking" to sides).
            // Allow a bit more tolerance so it's easier to land on mid-air platforms.
            bool comingFromAbove = prevBottom <= platformTop + 20.0f;
            bool movingDown = currBottom >= prevBottom;

            if (comingFromAbove && movingDown && currBottom > platformTop) {
                player_.setPosition({ player_.getPosition().x, platformTop - playerBounds.height });
                grounded = true;
                platform->applyTypeEffect(player_);
                // Refresh bounds after snapping
                playerBounds = player_.getBounds();
            }

            // Collision from below
            float prevTop = prevPlayerBounds.top;
            float platformBottom = platformBounds.top + platformBounds.height;
            float currTop = playerBounds.top;
            bool comingFromBelow = prevTop >= platformBottom - 20.0f;
            bool movingUp = currTop <= prevTop;

            if (comingFromBelow && movingUp && currTop < platformBottom) {
                player_.setPosition({ player_.getPosition().x, platformBottom });
                player_.setVelocityY(200.0f); // Bounce down
                if (lives_ > 1) {
                    lives_--;
                    hud_.updateLives(lives_);
                    ouchSound_.play();
                }
                else {
                    player_.die();
                }
                // Refresh bounds after snapping
                playerBounds = player_.getBounds();
            }
        }
    }

    // Simple safety: treat the main ground band as solid if the player is very close,
    // so the bunny does not fall through due to tiny numerical gaps.
    if (!grounded) {
        const float groundTopY = 600.0f;
        float playerBottom = playerBounds.top + playerBounds.height;
        if (playerBottom >= groundTopY - 4.0f && playerBottom <= groundTopY + 20.0f) {
            float snapY = groundTopY - playerBounds.height;
            player_.setPosition({ player_.getPosition().x, snapY });
            grounded = true;
            playerBounds = player_.getBounds();
        }
    }

    player_.setGrounded(grounded);

    comboTimer_ += 1.0f / 60.0f;
    if (comboTimer_ > 2.5f) {
        comboTimer_ = 0.0f;
        comboCount_ = 0;
    }

    for (auto& collectible : collectibles_) {
        if (collectible->isActive() && playerBounds.intersects(collectible->getBounds())) {
            collectible->collect();
            if (collectible->isPowerUp()) {
                switch (collectible->getType()) {
                case CollectibleType::Magnet:
                    magnetActive_ = true;
                    magnetTimer_ = 10.0f;
                    break;
                case CollectibleType::Shield:
                    shieldActive_ = true;
                    shieldTimer_ = 8.0f;
                    break;
                case CollectibleType::SpeedBoost:
                    player_.applySpeedBoost(2.0f, 5.0f);
                    break;
                case CollectibleType::DoublePoints:
                    doublePointsActive_ = true;
                    doublePointsTimer_ = 15.0f;
                    break;
                default:
                    break;
                }
            }
            else {
                comboCount_++;
                comboTimer_ = 0.0f;
                int basePoints = collectible->getPoints();
                float comboMultiplier = 1.0f + static_cast<float>(comboCount_) * 0.1f;
                if (doublePointsActive_) comboMultiplier *= 2.0f;
                int gained = static_cast<int>(std::round(basePoints * comboMultiplier));
                player_.addScore(gained);
                addScorePopup(collectible->getPosition(), gained);
            }
            collectSound_.play();
        }
    }

    for (auto& enemy : enemies_) {
        if (!enemy->isActive()) continue;
        if (playerBounds.intersects(enemy->getBounds())) {
            if (player_.getState() == PlayerState::Sliding || shieldActive_) {
                enemy->setActive(false);
                player_.addScore(50);
                comboCount_++;
                comboTimer_ = 0.0f;
                triggerScreenShake(6.0f, 0.2f);
                if (shieldActive_) {
                    shieldActive_ = false;
                    shieldTimer_ = 0.0f;
                }
                addScorePopup(enemy->getPosition(), 50);
                enemyHitSound_.play();
            }
            else {
                if (lives_ > 0) {
                    lives_--;
                    hud_.updateLives(lives_);
                    ouchSound_.play();
                }
                else {
                    player_.die();
                }
            }
        }
    }

    for (auto& projectile : projectiles_) {
        if (!projectile.active) continue;
        if (playerBounds.intersects(projectile.shape.getGlobalBounds())) {
            projectile.active = false;
            if (shieldActive_) {
                shieldActive_ = false;
                shieldTimer_ = 0.0f;
            }
            else {
                if (lives_ > 1) {
                    lives_--;
                    hud_.updateLives(lives_);
                    ouchSound_.play();
                }
                else {
                    player_.die();
                }
            }
        }
    }

    // Cactus & mushrooms: decorations with gameplay effects
    for (const auto& deco : decorations_) {
        const sf::Texture* tex = deco.sprite.getTexture();
        if (!tex) continue;
        if (!playerBounds.intersects(deco.sprite.getGlobalBounds())) continue;

        auto& rm = ResourceManager::getInstance();
        bool isCactus = rm.hasTexture("cactus") && (tex == &rm.getTexture("cactus"));
        bool isBrownMushroom = rm.hasTexture("decor_mushroom") && (tex == &rm.getTexture("decor_mushroom"));
        bool isRedMushroom = rm.hasTexture("mushroom_red") && (tex == &rm.getTexture("mushroom_red"));

        if (isRedMushroom) {
            // Red mushrooms are safe: no effect
            continue;
        }

        if (isBrownMushroom) {
            // Brown mushrooms: score penalty of 15, but never below 0
            int current = player_.getScore();
            if (current > 0) {
                int newScore = std::max(0, current - 15);
                player_.setScore(newScore);
            }
            continue;
        }

        if (isCactus) {
            // Cactus: harmful, lose life or die
            if (lives_ > 1) {
                lives_--;
                hud_.updateLives(lives_);
                ouchSound_.play();
            } else if (lives_ == 1) {
                lives_ = 0;
                hud_.updateLives(lives_);
                player_.die();
            }
        }
    }
}

void PlayingState::updateCamera() {
    float targetX = player_.getPosition().x - 300.0f;
    cameraSmoothX_ += (targetX - cameraSmoothX_) * 0.12f;
    cameraX_ = std::max(0.0f, cameraSmoothX_);

    if (cameraShakeTime_ > 0.0f) {
        cameraShakeTime_ -= 1.0f / 60.0f;
        float intensity = cameraShakeIntensity_ * (cameraShakeTime_ / std::max(0.01f, cameraShakeIntensity_));
        cameraShakeOffset_.x = randomFloat(rng_, -intensity, intensity);
        cameraShakeOffset_.y = randomFloat(rng_, -intensity * 0.5f, intensity * 0.5f);
    }
    else {
        cameraShakeOffset_ = sf::Vector2f(0.0f, 0.0f);
    }
}

void PlayingState::ensureGround() {
    float playerX = player_.getPosition().x;
    std::vector<std::tuple<float, float, float>> cactusRequests;
    while (groundGenerationX_ < playerX + 2400.0f) {
        float width = randomFloat(rng_, 420.0f, 800.0f);
        auto ground = std::make_unique<Platform>(
            sf::Vector2f(groundGenerationX_, 600.0f),
            sf::Vector2f(width, 140.0f),
            PlatformType::Normal);
        // Save cactus spawn requests for after all platforms are generated
        auto& rm = ResourceManager::getInstance();
        if (rm.hasTexture("cactus") && randomFloat(rng_, 0.0f, 1.0f) > 0.6f) {
            float cactusScale = 0.5f;
            float cactusHeight = rm.getTexture("cactus").getSize().y * cactusScale;
            float cactusX = groundGenerationX_ + randomFloat(rng_, 40.0f, width - 40.0f);
            float cactusY = 600.0f - cactusHeight;
            cactusRequests.emplace_back(cactusX, cactusY, cactusScale);
        }
        // Red mushrooms: safe decoration
        if (rm.hasTexture("mushroom_red") && randomFloat(rng_, 0.0f, 1.0f) > 0.6f) {
            Decoration deco;
            deco.sprite.setTexture(rm.getTexture("mushroom_red"));
            float mushScale = 0.5f;
            deco.sprite.setScale(mushScale, mushScale);
            float mushHeight = rm.getTexture("mushroom_red").getSize().y * mushScale;
            deco.sprite.setPosition(
                groundGenerationX_ + randomFloat(rng_, 40.0f, width - 40.0f),
                600.0f - mushHeight);
            deco.harmful = false;
            // Avoid overlap with existing decorations
            bool overlaps = false;
            for (const auto& existing : decorations_) {
                if (existing.sprite.getGlobalBounds().intersects(deco.sprite.getGlobalBounds())) {
                    overlaps = true;
                    break;
                }
            }
            if (!overlaps) {
                decorations_.push_back(deco);
            }
        }
        // Brown mushrooms: dangerous obstacle (score penalty)
        if (rm.hasTexture("decor_mushroom") && randomFloat(rng_, 0.0f, 1.0f) > 0.5f) {
            Decoration deco;
            deco.sprite.setTexture(rm.getTexture("decor_mushroom"));
            float mushScale = 0.5f;
            deco.sprite.setScale(mushScale, mushScale);
            float mushHeight = rm.getTexture("decor_mushroom").getSize().y * mushScale;
            deco.sprite.setPosition(
                groundGenerationX_ + randomFloat(rng_, 40.0f, width - 40.0f),
                600.0f - mushHeight);
            deco.harmful = true;
            // Avoid overlap with existing decorations
            bool overlaps = false;
            for (const auto& existing : decorations_) {
                if (existing.sprite.getGlobalBounds().intersects(deco.sprite.getGlobalBounds())) {
                    overlaps = true;
                    break;
                }
            }
            if (!overlaps) {
                decorations_.push_back(deco);
            }
        }

        platforms_.push_back(std::move(ground));
        groundGenerationX_ += width;
    }
    // After all ground and platforms are generated, place cacti only if not under a platform
    auto& rm = ResourceManager::getInstance();
    for (const auto& [cactusX, cactusY, cactusScale] : cactusRequests) {
        Decoration deco;
        deco.sprite.setTexture(rm.getTexture("cactus"));
        deco.sprite.setScale(cactusScale, cactusScale);
        deco.sprite.setPosition(cactusX, cactusY);
        deco.harmful = true;
        sf::FloatRect cactusBounds = deco.sprite.getGlobalBounds();
        bool underPlatform = false;
        for (const auto& plat : platforms_) {
            sf::FloatRect platBounds = plat->getBounds();
            // If platform is above cactus and horizontally overlaps
            if (platBounds.left < cactusX + cactusBounds.width &&
                platBounds.left + platBounds.width > cactusX &&
                platBounds.top + platBounds.height < 600.0f && // platform is above ground
                platBounds.top + platBounds.height > cactusY) {
                underPlatform = true;
                break;
            }
        }
        if (!underPlatform) {
            decorations_.push_back(deco);
        }
    }
}

void PlayingState::ensurePlatforms() {
    float playerX = player_.getPosition().x;
    while (generationX_ < playerX + 1200.0f) {
        float gap = randomFloat(rng_, platformMinGap_, platformMaxGap_);
        generationX_ += gap;
        float width = randomFloat(rng_, 160.0f, 360.0f);
        float height = randomFloat(rng_, 300.0f, 520.0f);

        PlatformType type = PlatformType::Normal;
        float roll = randomFloat(rng_, 0.0f, 1.0f);
        if (roll > 0.8f) type = PlatformType::MovingHorizontal;
        if (roll > 0.9f) type = PlatformType::Bouncy;
        if (roll > 0.95f) type = PlatformType::Ice;

        float movementRange = (type == PlatformType::MovingHorizontal || type == PlatformType::MovingVertical)
            ? randomFloat(rng_, 60.0f, 140.0f)
            : 0.0f;
        float movementSpeed = (movementRange > 0.0f) ? randomFloat(rng_, 50.0f, 90.0f) : 0.0f;

        auto platform = std::make_unique<Platform>(
            sf::Vector2f(generationX_, height),
            sf::Vector2f(width, 32.0f),
            type,
            movementRange,
            movementSpeed);

        spawnCollectiblePattern(*platform);

        // Add a visible spring on bouncy platforms so jumps feel more readable
        auto& rm = ResourceManager::getInstance();
        if (type == PlatformType::Bouncy && rm.hasTexture("spring")) {
            Decoration springDeco;
            springDeco.sprite.setTexture(rm.getTexture("spring"));
            float scale = 0.7f;
            springDeco.sprite.setScale(scale, scale);
            sf::Vector2f pos(
                generationX_ + width * 0.5f - (rm.getTexture("spring").getSize().x * scale / 2.0f),
                height - rm.getTexture("spring").getSize().y * scale);
            springDeco.sprite.setPosition(pos);
            springDeco.harmful = false;
            decorations_.push_back(springDeco);
        }

        // No extra platform walkers; enemy count is controlled by spawnEnemy()

        platforms_.push_back(std::move(platform));
        generationX_ += width;
    }
}

void PlayingState::cleanupOldEntities() {
    float cutoff = player_.getPosition().x - 900.0f;
    platforms_.erase(std::remove_if(platforms_.begin(), platforms_.end(),
        [&](const std::unique_ptr<Platform>& platform) {
            return (!platform->isActive() ||
                platform->getPosition().x + platform->getSize().x < cutoff);
        }),
        platforms_.end());

    projectiles_.erase(std::remove_if(projectiles_.begin(), projectiles_.end(),
        [&](const Projectile& projectile) {
            return !projectile.active;
        }),
        projectiles_.end());
}

void PlayingState::updatePowerUps(float deltaTime) {
    if (magnetActive_) {
        magnetTimer_ -= deltaTime;
        if (magnetTimer_ <= 0.0f) {
            magnetActive_ = false;
        }
    }
    if (shieldActive_) {
        shieldTimer_ -= deltaTime;
        if (shieldTimer_ <= 0.0f) {
            shieldActive_ = false;
        }
    }
    if (doublePointsActive_) {
        doublePointsTimer_ -= deltaTime;
        if (doublePointsTimer_ <= 0.0f) {
            doublePointsActive_ = false;
        }
    }
}

void PlayingState::updateDifficulty() {
    float difficulty = 1.0f + player_.getScore() / 800.0f + distance_ / 800.0f;
    player_.setRunSpeed(playerBaseSpeed_ + difficulty * 32.0f);
    platformMinGap_ = std::max(80.0f, 140.0f - difficulty * 6.0f);
    platformMaxGap_ = std::max(140.0f, 280.0f - difficulty * 10.0f);
}

void PlayingState::handleProjectiles(float deltaTime) {
    for (auto& projectile : projectiles_) {
        if (!projectile.active) continue;
        projectile.shape.move(projectile.velocity * deltaTime);
        if (projectile.shape.getPosition().x < cameraX_ - 200.0f) {
            projectile.active = false;
        }
    }
}

void PlayingState::triggerScreenShake(float intensity, float duration) {
    cameraShakeIntensity_ = intensity;
    cameraShakeTime_ = duration;
}

void PlayingState::updateScorePopups(float deltaTime) {
    for (auto& popup : scorePopups_) {
        popup.lifetime -= deltaTime;
        popup.text.move(popup.velocity * deltaTime);
        sf::Color color = popup.text.getFillColor();
        color.a = static_cast<sf::Uint8>(std::max(0.0f, popup.lifetime) / 1.0f * 255);
        popup.text.setFillColor(color);
    }
    scorePopups_.erase(std::remove_if(scorePopups_.begin(), scorePopups_.end(),
        [](const ScorePopup& popup) { return popup.lifetime <= 0.0f; }),
        scorePopups_.end());
}

void PlayingState::addScorePopup(const sf::Vector2f& position, int points) {
    if (!font_) return;
    ScorePopup popup;
    popup.text.setFont(*font_);
    popup.text.setCharacterSize(24);
    popup.text.setFillColor(sf::Color(255, 255, 255));
    popup.text.setOutlineColor(sf::Color::Black);
    popup.text.setOutlineThickness(2.0f);
    popup.text.setString("+" + std::to_string(points));
    popup.text.setPosition(position);
    popup.velocity = sf::Vector2f(0.0f, -30.0f);
    popup.lifetime = 1.0f;
    scorePopups_.push_back(popup);
}

int PlayingState::loadHighScore() const {
    std::ifstream file("highscore.dat");
    int score = 0;
    if (file.is_open()) {
        file >> score;
    }
    return score;
}

void PlayingState::saveHighScore() const {
    std::ofstream file("highscore.dat", std::ios::trunc);
    if (file.is_open()) {
        file << highScore_;
    }
}
