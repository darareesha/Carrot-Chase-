#pragma once

#include <SFML/Graphics.hpp>
#include <vector>

class Animation {
public:
    Animation();

    void addFrame(const sf::IntRect& frame);
    void setFrameTime(float time);
    void setLooping(bool looping);

    void update(float deltaTime);
    void reset();

    sf::IntRect getCurrentFrame() const;
    bool isFinished() const;

private:
    std::vector<sf::IntRect> frames_;
    float frameTime_;
    float currentTime_;
    size_t currentFrame_;
    bool looping_;
    bool finished_;
};

