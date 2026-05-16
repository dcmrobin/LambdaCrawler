#include "game.h"

extern int LOGICAL_WIDTH;
extern int LOGICAL_HEIGHT;

void game_setup() {
    for (const auto& name : sprite_names) {
        LoadSprite(name, ("assets/sprites/" + name + ".png").c_str());
    }
    SetCustomCursor("cursor_normal", 0, 0); // Set hotspot (top-left corner)
    ShowSystemCursor(false);
    LoadFont("8bitMageFont", "assets/fonts/DiaryOfAn8BitMage-lYDD.ttf", 24);
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
    gameState = STATE_MENU;
}

void game_loop() {
    RenderGame();
    UpdateGame();
}

void UpdateGame() {
    if (gameState == STATE_RUN) {
        UpdateKeyStates();
        HandlePlayerInput();
        UpdateMap();
        UpdateCamera();
    } else if (gameState == STATE_MENU) {
        UpdateMenu();
    }
}

void RenderGame() {
    if (gameState == STATE_RUN) {
        RenderMap();
        RenderPlayer();
    } else if (gameState == STATE_MENU) {
        RenderMenu();
    }
    DrawCustomCursor();
}

void UpdateMenu() {
    //
}