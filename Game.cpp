#include "Game.h"
#include "MenuState.h"
#include <iostream>

Game::Game()
    : window_(sf::VideoMode(1280, 720), "Cute Platformer Game", sf::Style::Close)
    , deltaTime_(0.0f)
    , running_(true) {

    window_.setFramerateLimit(60);
    window_.setVerticalSyncEnabled(true);

    // Load resources
    auto& rm = ResourceManager::getInstance();

    // Try to load default font (Arial fallback)
    // In a real game, you'd load your custom font
    // For now, we'll create a simple system font or use SFML's default

    // Load background music (if available)
    // Note: Add your background music file and uncomment this
    // if (backgroundMusic_.openFromFile("bg_music.ogg")) {
    //     backgroundMusic_.setLoop(true);
    //     backgroundMusic_.setVolume(30.0f);
    //     backgroundMusic_.play();
    // }
}

Game::~Game() {
    while (!states_.empty()) {
        states_.pop();
    }
}

void Game::run() {
    // Start with menu state
    changeState(std::make_unique<MenuState>());

    while (running_ && window_.isOpen()) {
        deltaTime_ = clock_.restart().asSeconds();

        handleEvents();
        update(deltaTime_);
        render();
    }
}

void Game::handleEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window_.close();
            running_ = false;
        }

        if (!states_.empty()) {
            states_.top()->handleInput(event);
        }
    }
}

void Game::update(float deltaTime) {
    if (!states_.empty()) {
        states_.top()->update(deltaTime);
    }
}

void Game::render() {
    window_.clear(sf::Color(135, 206, 250)); // Sky blue background

    if (!states_.empty()) {
        states_.top()->render(window_);
    }

    window_.display();
}

void Game::changeState(std::unique_ptr<GameState> state) {
    if (!states_.empty()) {
        states_.pop();
    }

    if (state) {
        state->setGame(this);
        states_.push(std::move(state));
    }
}

void Game::pushState(std::unique_ptr<GameState> state) {
    if (state) {
        state->setGame(this);
        states_.push(std::move(state));
    }
}

void Game::popState() {
    if (!states_.empty()) {
        states_.pop();
    }
}

