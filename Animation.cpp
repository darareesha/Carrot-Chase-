#include "Animation.h"

Animation::Animation()
    : frameTime_(0.1f)
    , currentTime_(0.0f)
    , currentFrame_(0)
    , looping_(true)
    , finished_(false) {
}

void Animation::addFrame(const sf::IntRect& frame) {
    frames_.push_back(frame);
}

void Animation::setFrameTime(float time) {
    frameTime_ = time;
}

void Animation::setLooping(bool looping) {
    looping_ = looping;
}

void Animation::update(float deltaTime) {
    if (frames_.empty()) return;

    currentTime_ += deltaTime;

    if (currentTime_ >= frameTime_) {
        currentTime_ = 0.0f;
        currentFrame_++;

        if (currentFrame_ >= frames_.size()) {
            if (looping_) {
                currentFrame_ = 0;
            }
            else {
                currentFrame_ = frames_.size() - 1;
                finished_ = true;
            }
        }
    }
}

void Animation::reset() {
    currentTime_ = 0.0f;
    currentFrame_ = 0;
    finished_ = false;
}

sf::IntRect Animation::getCurrentFrame() const {
    if (frames_.empty()) {
        return sf::IntRect(0, 0, 0, 0);
    }
    return frames_[currentFrame_];
}

bool Animation::isFinished() const {
    return finished_;
}

