#include "ResourceManager.h"
#include <iostream>

ResourceManager& ResourceManager::getInstance() {
    static ResourceManager instance;
    return instance;
}

bool ResourceManager::loadTexture(const std::string& name, const std::string& path) {
    auto texture = std::make_unique<sf::Texture>();
    if (!texture->loadFromFile(path)) {
        std::cerr << "Failed to load texture: " << path << std::endl;
        return false;
    }
    textures_[name] = std::move(texture);
    return true;
}

sf::Texture& ResourceManager::getTexture(const std::string& name) {
    if (textures_.find(name) == textures_.end()) {
        throw std::runtime_error("Texture not found: " + name);
    }
    return *textures_[name];
}

bool ResourceManager::hasTexture(const std::string& name) const {
    return textures_.find(name) != textures_.end();
}

bool ResourceManager::loadSoundBuffer(const std::string& name, const std::string& path) {
    auto soundBuffer = std::make_unique<sf::SoundBuffer>();
    if (!soundBuffer->loadFromFile(path)) {
        std::cerr << "Failed to load sound: " << path << std::endl;
        return false;
    }
    soundBuffers_[name] = std::move(soundBuffer);
    return true;
}

sf::SoundBuffer& ResourceManager::getSoundBuffer(const std::string& name) {
    if (soundBuffers_.find(name) == soundBuffers_.end()) {
        throw std::runtime_error("SoundBuffer not found: " + name);
    }
    return *soundBuffers_[name];
}

bool ResourceManager::hasSoundBuffer(const std::string& name) const {
    return soundBuffers_.find(name) != soundBuffers_.end();
}

bool ResourceManager::loadFont(const std::string& name, const std::string& path) {
    auto font = std::make_unique<sf::Font>();
    if (!font->loadFromFile(path)) {
        std::cerr << "Failed to load font: " << path << std::endl;
        return false;
    }
    fonts_[name] = std::move(font);
    return true;
}

sf::Font& ResourceManager::getFont(const std::string& name) {
    if (fonts_.find(name) == fonts_.end()) {
        throw std::runtime_error("Font not found: " + name);
    }
    return *fonts_[name];
}

bool ResourceManager::hasFont(const std::string& name) const {
    return fonts_.find(name) != fonts_.end();
}

void ResourceManager::clear() {
    textures_.clear();
    soundBuffers_.clear();
    fonts_.clear();
}

