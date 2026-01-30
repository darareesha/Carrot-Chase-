#include "Game.h"
#include "ResourceManager.h"
#include <iostream>

int main() {
    try {
        // Load resources
        auto& rm = ResourceManager::getInstance();

        // Load textures - using actual asset files from project root
        // Player sprites (bunny character)
        rm.loadTexture("player_idle", "bunny1_stand.png");
        rm.loadTexture("player_run", "bunny1_walk1.png");
        rm.loadTexture("player_run2", "bunny1_walk2.png"); // Second walk frame
        rm.loadTexture("player_jump", "bunny1_jump.png");
        rm.loadTexture("player_slide", "bunny1_ready.png"); // Using ready pose for slide
        rm.loadTexture("player_death", "bunny1_hurt.png");

        // Enemy sprites
        rm.loadTexture("enemy", "spikeMan_stand.png");
        rm.loadTexture("enemy_walk1", "spikeMan_walk1.png");
        rm.loadTexture("enemy_walk2", "spikeMan_walk2.png");
        rm.loadTexture("enemy_fly", "flyMan_fly.png");
        // FlyMan animation frames for sky enemies
        rm.loadTexture("flyMan_fly", "flyMan_fly.png");
        rm.loadTexture("flyMan_still_fly", "flyMan_still_fly.png");

        // Collectibles (carrots & mushrooms)
        rm.loadTexture("carrot", "carrot.png");                 // 1 point
        rm.loadTexture("carrot_gold", "carrot_gold.png");       // golden carrot
        rm.loadTexture("mushroom_red", "mushroom_red.png");     // candy-style collectible
        rm.loadTexture("coin_gold", "coin_gold.png");

        // Platforms
        rm.loadTexture("platform", "ground_grass.png");
        rm.loadTexture("platform_cake", "ground_cake.png");
        rm.loadTexture("platform_sand", "ground_sand.png");

        // Decorations
        rm.loadTexture("cactus", "cactus.png");
        rm.loadTexture("decor_mushroom", "mushroom_brown.png");
        rm.loadTexture("spring", "spring.png");
        rm.loadTexture("cloud", "cloud.png");

        // Background layers
        rm.loadTexture("bg_layer0", "bg_layer1.png"); // Far background
        rm.loadTexture("bg_layer1", "bg_layer2.png"); // Mid background
        rm.loadTexture("bg_layer2", "bg_layer3.png"); // Near background

        // UI & cursor textures
        rm.loadTexture("button_play", "buttonSquare_brown.png");
        rm.loadTexture("button_exit", "buttonSquare_grey.png");
        rm.loadTexture("cursor_hand", "cursorHand_beige.png");
            rm.loadTexture("lifeline_icon", "sun1.png");

        // Load sounds (if available)
        rm.loadSoundBuffer("jump", "impactPlate_medium_004.ogg");
        rm.loadSoundBuffer("hit", "impactPlate_medium_004.ogg");
        rm.loadSoundBuffer("ouch", "ouch.WAV");
        // Note: Add more sound files as needed

        // Load font
        rm.loadFont("default", "Baloo2-VariableFont_wght.ttf");

        // Create and run game
        Game game;
        game.run();

    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

