#include "Background.h"

Background::Background()
    : scrollSpeed_(100.0f)
    , totalWidth_(1280.0f) {

    auto& rm = ResourceManager::getInstance();

    // Create parallax layers (back to front)
    // Layer 0: Far background (sky/clouds) - slowest
    if (rm.hasTexture("bg_layer0")) {
        Layer layer0;
        layer0.sprite.setTexture(rm.getTexture("bg_layer0"));
        layer0.parallaxFactor = 0.2f;
        layer0.offset = 0.0f;
        layers_.push_back(layer0);
    }

    // Layer 1: Mid background (mountains/buildings) - medium
    if (rm.hasTexture("bg_layer1")) {
        Layer layer1;
        layer1.sprite.setTexture(rm.getTexture("bg_layer1"));
        layer1.parallaxFactor = 0.4f;
        layer1.offset = 0.0f;
        layers_.push_back(layer1);
    }

    // Layer 2: Near background (trees/decorations) - faster
    if (rm.hasTexture("bg_layer2")) {
        Layer layer2;
        layer2.sprite.setTexture(rm.getTexture("bg_layer2"));
        layer2.parallaxFactor = 0.6f;
        layer2.offset = 0.0f;
        layers_.push_back(layer2);
    }

    // If no textures loaded, create colored backgrounds
    if (layers_.empty()) {
        // Create a simple gradient background
        Layer skyLayer;
        skyLayer.parallaxFactor = 0.1f;
        skyLayer.offset = 0.0f;
        layers_.push_back(skyLayer);
    }

    // Scale layers to cover screen
    for (auto& layer : layers_) {
        if (layer.sprite.getTexture()) {
            auto texSize = layer.sprite.getTexture()->getSize();
            float scaleY = 720.0f / texSize.y;
            layer.sprite.setScale(scaleY, scaleY);
            totalWidth_ = texSize.x * scaleY;
        }
    }
}

void Background::update(float deltaTime, float cameraX) {
    for (auto& layer : layers_) {
        // Calculate parallax offset
        layer.offset = cameraX * layer.parallaxFactor;

        // Wrap around for seamless scrolling
        if (layer.offset <= -totalWidth_) {
            layer.offset += totalWidth_;
        }
    }
}

void Background::render(sf::RenderWindow& window) {
    for (auto& layer : layers_) {
        if (layer.sprite.getTexture()) {
            // Draw multiple copies for seamless scrolling
            for (int i = -1; i <= 1; i++) {
                float xPos = layer.offset + (i * totalWidth_);
                layer.sprite.setPosition(xPos, 0.0f);
                window.draw(layer.sprite);
            }
        }
        else {
            // Draw gradient background if no texture
            sf::RectangleShape bg;
            bg.setSize(sf::Vector2f(1280.0f, 720.0f));
            bg.setPosition(layer.offset, 0.0f);
            bg.setFillColor(sf::Color(135, 206, 250)); // Sky blue
            window.draw(bg);
        }
    }
}

void Background::setScrollSpeed(float speed) {
    scrollSpeed_ = speed;
}

