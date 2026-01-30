#pragma once

#include <SFML/Graphics.hpp>
#include <vector>
#include "ResourceManager.h"

class Background {
public:
    Background();

    void update(float deltaTime, float cameraX);
    void render(sf::RenderWindow& window);

    void setScrollSpeed(float speed);

private:
    struct Layer {
        sf::Sprite sprite;
        float parallaxFactor;
        float offset;
    };

    std::vector<Layer> layers_;
    float scrollSpeed_;
    float totalWidth_;
};

