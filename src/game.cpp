#include "game.h"
#include "GFX.h"
#include "HelperFunctions.h"
#include "Player.h"
#include "Map.h"
#include "LambdaLogic.h"
#include <iostream>

void game_setup() {
    for (const auto& name : sprite_names) {
        LoadSprite(name, ("assets/" + name + ".png").c_str());
    }
    InitializeRoom();
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
    RenderMap();
    DrawSprite(player.spriteName, player.x, player.y, 16, 16);

    auto lambda = GenerateReducibleLambda(3);
    std::cout << "Generated Lambda: " << LambdaToString(lambda) << std::endl;
    RenderLambdaGrid(lambda, 10, 20, 12, 8, 10, 10);
}