#include "game.h"

std::vector<Button> buttons = {{}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},};
std::vector<Slider> sliders = {{}, {}, {}, {}, {},};
bool showSettings = false; // Flag to track whether the settings panel should show
bool aSettingHasChanged = false; // Flag to track whether a setting has changed, so that the apply button can be activated when a setting is changed
int originalVolumeSliderValue = 45; // Int to store the original volume slider value so that it can be reverted to if the player exits the settings panel without applying any changes
int mainVolume = 45; // Int to store the actual volume of the game
bool settingsApplied = false; // Flag to track whether the player has applied any settings changes, so that changes can revert if nothing has been applied in the settings panel

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
    DrawCustomCursor();
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
        UpdateSliders();
    }
}

void RenderGame() {
    if (gameState == STATE_RUN) {
        RenderMap(); // Render map and player if the player has clicked play and therefore is in STATE_RUN
        RenderPlayer();
    } else if (gameState == STATE_MENU) {
        RenderMenu(); // Render the title screen menu, as the value of gameState is STATE_MENU on bootup
        RenderButtons(); // Render buttons on top of everything, as they won't need to be hidden by anything and nothing will be on top of them
        RenderSliders();
    }
}

void UpdateMenu() {
    buttons[0].text = "Start Game"; // Assign what text should be drawn on the button
    buttons[0].action = BTN_START_GAME;// Assign what action the button should trigger by picking a value in the ButtonAction enum
    // Position the button (LOGICAL_WIDTH & LOGICAL_HEIGHT are the width and height of the application viewport)
    buttons[0].x = (LOGICAL_WIDTH / 2 - buttons[0].width / 2)-85;
    buttons[0].y = (LOGICAL_HEIGHT / 2 - buttons[0].height / 2)-20;

    // Only set the load game button's text to "Load Game" if it isn't already set to a feedback message
    buttons[1].text = (buttons[1].text != "Game Loaded" && buttons[1].text != "No Save File") ? "Load Game" : buttons[1].text;
    buttons[1].action = BTN_LOAD_GAME;
    buttons[1].x = (LOGICAL_WIDTH / 2 - buttons[1].width / 2)-85;
    buttons[1].y = (LOGICAL_HEIGHT / 2 - buttons[1].height / 2)+5;

    buttons[2].text = "Settings";
    buttons[2].action = BTN_SHOW_SETTINGS;
    buttons[2].x = (LOGICAL_WIDTH / 2 - buttons[2].width / 2)-85;
    buttons[2].y = (LOGICAL_HEIGHT / 2 - buttons[2].height / 2)+30;

    buttons[3].text = "Quit";
    buttons[3].action = BTN_QUIT_APPLICATION;
    buttons[3].x = (LOGICAL_WIDTH / 2 - buttons[3].width / 2)-85;
    buttons[3].y = (LOGICAL_HEIGHT / 2 - buttons[3].height / 2)+55;
}