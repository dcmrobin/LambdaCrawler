#include "game.h"
#include "GFX.h"
#include "HelperFunctions.h"
#include "Player.h"

void game_setup() {
    for (const auto& name : sprite_names) {
        LoadSprite(name, ("assets/" + name + ".png").c_str());
    }
}

void game_loop() {
    UpdateGame();
    RenderGame();
}

void UpdateGame() {
    UpdateKeyStates();
    HandlePlayerInput();
}

void RenderGame() {
    FillRect(0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT, { 20, 20, 20, 255 }); // Clear screen
    DrawSprite(player.spriteName, player.x, player.y, 16, 16);
}