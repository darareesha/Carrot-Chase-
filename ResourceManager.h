#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>

class ResourceManager {
public:
    static ResourceManager& getInstance();

    // Texture management
    bool loadTexture(const std::string& name, const std::string& path);
    sf::Texture& getTexture(const std::string& name);
    bool hasTexture(const std::string& name) const;

    // Sound management
    bool loadSoundBuffer(const std::string& name, const std::string& path);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);
    bool hasSoundBuffer(const std::string& name) const;

    // Font management
    bool loadFont(const std::string& name, const std::string& path);
    sf::Font& getFont(const std::string& name);
    bool hasFont(const std::string& name) const;

    // Cleanup
    void clear();

private:
    ResourceManager() = default;
    ~ResourceManager() = default;
    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;

    std::map<std::string, std::unique_ptr<sf::Texture>> textures_;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> soundBuffers_;
    std::map<std::string, std::unique_ptr<sf::Font>> fonts_;
};
