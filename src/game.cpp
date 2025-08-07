#include "game.h"
#include "GFX.h"
#include "HelperFunctions.h"
#include "Player.h"
#include "Map.h"
#include "LambdaLogic.h"
#include <iostream>
#include <algorithm>

// Camera state
int cameraX = 0;
int cameraY = 0;
float cameraXF = 0.0f;
float cameraYF = 0.0f;
const float CAMERA_LERP = 0.15f;

extern int LOGICAL_WIDTH;
extern int LOGICAL_HEIGHT;

void game_setup() {
    for (const auto& name : sprite_names) {
        LoadSprite(name, ("assets/" + name + ".png").c_str());
    }
    InitializeRoom();
    // Center camera at start if needed
    if (currentLevel > 0) {
        cameraXF = player.x + player.hitbox.width / 2 - LOGICAL_WIDTH / 2;
        cameraYF = player.y + player.hitbox.height / 2 - LOGICAL_HEIGHT / 2;
        cameraX = static_cast<int>(cameraXF);
        cameraY = static_cast<int>(cameraYF);
    } else {
        cameraXF = cameraX = 0;
        cameraYF = cameraY = 0;
    }
}

void game_loop() {
    UpdateGame();
    RenderGame();
}

void UpdateGame() {
    UpdateKeyStates();
    HandlePlayerInput();
    // Camera follows player smoothly if currentLevel > 0
    if (currentLevel > 0) {
        int targetX = player.x + player.hitbox.width / 2 - LOGICAL_WIDTH / 2;
        int targetY = player.y + player.hitbox.height / 2 - LOGICAL_HEIGHT / 2;
        // Clamp target
        targetX = std::max(0, std::min(targetX, (mapWidth * tileSize) - LOGICAL_WIDTH));
        targetY = std::max(0, std::min(targetY, (mapHeight * tileSize) - LOGICAL_HEIGHT));
        // Smoothly interpolate
        cameraXF += (targetX - cameraXF) * CAMERA_LERP;
        cameraYF += (targetY - cameraYF) * CAMERA_LERP;
        cameraX = static_cast<int>(cameraXF);
        cameraY = static_cast<int>(cameraYF);
    } else {
        cameraXF = cameraX = 0;
        cameraYF = cameraY = 0;
    }
}

void RenderGame() {
    RenderMap();
    RenderPlayer();

    auto lambda = GenerateReducibleLambda(3);
    RenderLambdaGrid(lambda, 10, 20, 12, 8, 10, 10);
}