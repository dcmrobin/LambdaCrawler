#include "game.h"
#include "Sprites.h"

void game_setup() {
    for (int i = 0; i < NUM_SPRITES; ++i) {
        LoadSprite(sprite_names[i], ("assets/" + sprite_names[i] + ".png").c_str());
    }
}

void game_loop() {
    // Draw sprite at LOGICAL (100, 100) with automatic scaling
    // Parameters: (sprite_name, logical_x, logical_y, logical_width, logical_height)
    DrawSprite("player_idle", 100, 100, 16, 16);
}