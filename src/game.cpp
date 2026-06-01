#include "game.h"

std::vector<Button> buttons = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},};

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
        UpdateButtons();
    }
}

void RenderGame() {
    if (gameState == STATE_RUN) {
        RenderMap();
        RenderPlayer();
    } else if (gameState == STATE_MENU) {
        RenderMenu();
        RenderButtons();
    }
    DrawCustomCursor();
}

void UpdateMenu() {
    buttons[0].text = "Start Game"; // Assign what text should be drawn on the button
    buttons[0].action = START_GAME;// Assign what action the button should trigger by picking a value in the ButtonAction enum
    // Position the button (LOGICAL_WIDTH & LOGICAL_HEIGHT are the width and height of the application viewport)
    buttons[0].x = (LOGICAL_WIDTH / 2 - buttons[0].width / 2)-85;
    buttons[0].y = (LOGICAL_HEIGHT / 2 - buttons[0].height / 2)-20;

    buttons[1].text = "Load Game";
    buttons[1].action = LOAD_GAME;
    buttons[1].x = (LOGICAL_WIDTH / 2 - buttons[1].width / 2)-85;
    buttons[1].y = (LOGICAL_HEIGHT / 2 - buttons[1].height / 2)+5;

    buttons[2].text = "Settings";
    buttons[2].action = SHOW_SETTINGS;
    buttons[2].x = (LOGICAL_WIDTH / 2 - buttons[2].width / 2)-85;
    buttons[2].y = (LOGICAL_HEIGHT / 2 - buttons[2].height / 2)+30;

    buttons[3].text = "Quit";
    buttons[3].action = QUIT_APPLICATION;
    buttons[3].x = (LOGICAL_WIDTH / 2 - buttons[3].width / 2)-85;
    buttons[3].y = (LOGICAL_HEIGHT / 2 - buttons[3].height / 2)+55;
}