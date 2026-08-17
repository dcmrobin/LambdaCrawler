#include "HelperFunctions.h"

KeyStates keystates;
GameStates gameState = STATE_MENU;
bool is_fullscreen = false;
// Camera state
float cameraXF = 0.0f;
float cameraYF = 0.0f;
const float CAMERA_LERP = 0.15f;
TTF_Font* font = TTF_OpenFont("assets/fonts/DiaryOfAn8BitMage-lYDD.ttf", 24);

void UpdateKeyStates() {
  // Save previous states
  keystates.upPressedPrev = keystates.upPressed;
  keystates.downPressedPrev = keystates.downPressed;
  keystates.zPressedPrev = keystates.zPressed;
  keystates.xPressedPrev = keystates.xPressed;
  keystates.leftPressedPrev = keystates.leftPressed;
  keystates.rightPressedPrev = keystates.rightPressed;
  keystates.startPressedPrev = keystates.startPressed;
  keystates.enterPressedPrev = keystates.enterPressed;

  // Read current states
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  keystates.upPressed = keystate[SDL_SCANCODE_UP];
  keystates.downPressed = keystate[SDL_SCANCODE_DOWN];
  keystates.leftPressed = keystate[SDL_SCANCODE_LEFT];
  keystates.rightPressed = keystate[SDL_SCANCODE_RIGHT];
  keystates.zPressed = keystate[SDL_SCANCODE_Z];
  keystates.xPressed = keystate[SDL_SCANCODE_X];
  keystates.startPressed = keystate[SDL_SCANCODE_ESCAPE];
  keystates.enterPressed = keystate[SDL_SCANCODE_RETURN];
}

void UpdateCamera() {
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

bool IsKeyPressed(InputKey key, bool held) {
    if (!held)
    {
        switch (key) {
            case KEY_UP:    return keystates.upPressed && !keystates.upPressedPrev;
            case KEY_DOWN:  return keystates.downPressed && !keystates.downPressedPrev;
            case KEY_LEFT:  return keystates.leftPressed && !keystates.leftPressedPrev;
            case KEY_RIGHT: return keystates.rightPressed && !keystates.rightPressedPrev;
            case KEY_Z:     return keystates.zPressed && !keystates.zPressedPrev;
            case KEY_X:     return keystates.xPressed && !keystates.xPressedPrev;
            case KEY_START: return keystates.startPressed && !keystates.startPressedPrev;
            case KEY_ENTER: return keystates.enterPressed && !keystates.enterPressedPrev;
            default:        return false;
        }
    }
    else
    {
        switch (key) {
            case KEY_UP:    return keystates.upPressed;
            case KEY_DOWN:  return keystates.downPressed;
            case KEY_LEFT:  return keystates.leftPressed;
            case KEY_RIGHT: return keystates.rightPressed;
            case KEY_Z:     return keystates.zPressed;
            case KEY_X:     return keystates.xPressed;
            case KEY_START: return keystates.startPressed;
            case KEY_ENTER: return keystates.enterPressed;
            default:        return false;
        }
    }
}

bool Intersects(Hitbox& current, Hitbox other){
    return (current.x < other.x + other.width &&
            current.x + current.width > other.x &&
            current.y < other.y + other.height &&
            current.y + current.height > other.y);
}

void ChangeTile(int x, int y, bool solid, TileType type) {
    for (auto& tile : mapTiles) {
        if (tile.x == x && tile.y == y) {
            tile.solid = solid;
            tile.type = type;
            return;
        }
    }
    // If not found, create a new tile
    Tile newTile;
    newTile.x = x;
    newTile.y = y;
    newTile.hitbox = {x, y, tileSize, tileSize};
    newTile.solid = solid;
    newTile.type = type;
    mapTiles.push_back(newTile);
}

void UpdateButtons() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    for (auto& button : buttons) {
        if (button.text == "null") continue; // Skip buttons that aren't initialized

        if (!button.active) { // Only update this button if it is active
            button.spriteName = "button_pressed"; // Change the buttons sprite to "greyed-out" (the pressed sprite) and skip it
            continue;
        }

        // Check if another button is already pressed, and if so, don't update this one
        bool aButtonIsAlreadyPressed = false;
        for (auto& b : buttons) {
            if (b.pressed) {
                aButtonIsAlreadyPressed = true;
            }
        }
        if (aButtonIsAlreadyPressed && IsMouseButtonPressed(1)) {
            continue;
        }

        // Check if a slider is being cahnged, and if so, don't update this button
        bool aSliderIsAlreadyBeingChanged = false;
        for (auto& s : sliders) {
            if (s.handleGrabbed) {
                aSliderIsAlreadyBeingChanged = true;
            }
        }
        if (aSliderIsAlreadyBeingChanged && IsMouseButtonPressed(1)) {
            continue;
        }

        // Convert global mouse position to viewport mouse position
        float logical_x, logical_y;
        SDL_RenderWindowToLogical(renderer, mouseX, mouseY, &logical_x, &logical_y);
        // Check if the mouse pointer position is inside the bounds of the button (this line was too long to fit on the screen all at once)
        button.highlighted = ((int)(logical_x - cursor_hotspot.x-5) >= button.x && (int)(logical_x - cursor_hotspot.x+5) <= button.x + button.width &&
                          (int)(logical_y - cursor_hotspot.y-5) >= button.y && (int)(logical_y - cursor_hotspot.y+5) <= button.y + button.height);
        
        // Change button pressed flags according to left mouse button presses depending on if the button is highlighted over in the first place
        if (button.highlighted && IsMouseButtonPressed(1)) {
            button.pressed = true;
        } else if (button.pressed && !IsMouseButtonPressed(1) && button.highlighted) {
            button.pressed = false;
            TriggerButtonAction(button.action); // Trigger the button's action
        } else {
            button.pressed = false;
        }
        // Change button sprites to highlighted/pressed/idle according to its highlighted/pressed flags
        if (!button.pressed && !button.highlighted) {
            button.spriteName = button.originalSpriteName;
        } else if (button.highlighted && !button.pressed) {
            button.spriteName = button.originalSpriteName + "_highlighted";
        } else if (button.pressed) {
            button.spriteName = button.originalSpriteName + "_pressed";
        }
    }
}

bool IsMouseButtonPressed(int button) {
    Uint32 mouseState = SDL_GetMouseState(NULL, NULL);
    return (mouseState & SDL_BUTTON(button)) != 0;
}

void TriggerButtonAction(ButtonAction action) {
    switch (action) { // Look through the ButtonAction enum to find the one that was referenced in the function call
        case BTN_QUIT_APPLICATION:
            std::exit(0); // Cleanly exit the application
            break;
        case BTN_START_GAME:
            for (auto &button : buttons) {
                button.text = "null"; // Loop through all of the buttons and disable them, as they are not present in the initial game view
            }
            gameState = STATE_RUN; // Change the gameState to playing the game so that rendering logic changes from rendering the menu to rendering the game
            break;
        case BTN_LOAD_GAME:
            if (std::filesystem::exists("save.dat")) { // Check if the save file exists
                buttons[1].text = "Game Loaded"; // Show confirmation that the save file has been loaded
                // Placeholder, would read the contents of the save file and load it into memory here
            } else {
                buttons[1].text = "No Save File"; // Show to the player that no save file exists
            }
            buttons[1].active = false; // Load game button disables no matter the outcome so that the player doesn't press it more than necessary
            break;
        case BTN_SHOW_SETTINGS:
            showSettings = true; // Set the showSettings flag to true in order to tell RenderMenu() to draw the settings panel
            settingsApplied = false; // Reset the settingsApplied flag to false, as the player has just opened the settings panel and hasn't applied any changes yet
            originalVolumeSliderValue = sliders[0].value; // Store the original volume slider value so that it can be reverted
            buttons[9].active = false; // Grey out the apply button, as the player hasn't changed any settings yet
            for (int i = 0; i < 4; i++) {
                buttons[i].active = false; // Loop through the four first buttons (the four on the title screen) and deactivate them - they are not part of the settings panel
            }
            // Set the names of all the settings panel's buttons once
            buttons[4].spriteName = "exitButton";
            buttons[4].originalSpriteName = "exitButton";
            break;
        case BTN_HIDE_SETTINGS:
            showSettings = false; // Set the showSettings flag to false in order to tell RenderMenu() to stop drawing the settings panel
            if (!settingsApplied) {
                sliders[0].value = originalVolumeSliderValue; // Revert the volume slider to its original value if the player exits the settings panel without applying any changes
            }
            for (int i = 4; i < (int)buttons.size(); i++) {
                buttons[i].text = "null"; // Loop through all of the buttons on the settings panel and set them to null so that they won't be drawn, as the panel has been closed
            }
            for (int i = 0; i < 4; i++) {
                buttons[i].active = true; // Loop through the four first buttons (the four on the title screen) and reactivate them
            }
            break;
        case BTN_FULLSCREEN:
            SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP); // Set the window to fullscreen mode
            is_fullscreen = true;
            break;
        case BTN_WINDOWED:
            SDL_SetWindowFullscreen(window, 0); // Set the window to windowed mode
            is_fullscreen = false;
            break;
        case BTN_LOAD_SETTINGS:
            // Placeholder for loading settings from a file
            break;
        case BTN_SAVE_SETTINGS:
            // Placeholder for saving settings to a file
            break;
        case BTN_APPLY_SETTINGS:
            buttons[9].active = false; // Player has just applied settings, no need for apply button to be active anymore until a setting is changed again
            settingsApplied = true; // Player has applied settings, so that if they exit the settings panel without applying any changes, the changes will not revert
            originalVolumeSliderValue = sliders[0].value; // Prevent the volume slider from reverting, as it has been applied
            mainVolume = originalVolumeSliderValue; // Set the main volume to the value of the volume slider
            break;
        default:
            return;
    }
}

void UpdateSliders() {
    int mouseX, mouseY;
    SDL_GetMouseState(&mouseX, &mouseY);
    for (auto& slider : sliders) {
        if (!slider.handleActive) {continue;} // Skip inactive sliders

        // Check if a button is being pressed, and if so, don't update this slider
        bool aButtonIsAlreadyPressed = false;
        for (auto& b : buttons) {
            if (b.pressed) {
                aButtonIsAlreadyPressed = true;
            }
        }
        if (aButtonIsAlreadyPressed && IsMouseButtonPressed(1)) {
            continue;
        }

        // Check if a different slider is being changed, and if so, don't update this one
        bool aSliderIsAlreadyBeingChanged = false;
        for (auto& s : sliders) {
            if (s.handleGrabbed && s.text != slider.text) { // Make sure to ignore this slider, otherwise this slider will not be updated as soon as the player grabs its handle
                aSliderIsAlreadyBeingChanged = true;
            }
        }
        if (aSliderIsAlreadyBeingChanged && IsMouseButtonPressed(1)) {
            continue;
        }

        // Normalize slider's value for drawing the handle, as the width will not always be a clean 100
        float normalizedSliderValue = (slider.value / slider.maxValue) * slider.width;

        // Convert global mouse position to viewport mouse position
        float logical_x, logical_y;
        SDL_RenderWindowToLogical(renderer, mouseX, mouseY, &logical_x, &logical_y);

        // Declare handle attributes for easy access
        float handleWidth = 20;
        float handleHeight = slider.height*2;
        float handleX = (slider.x + normalizedSliderValue) - handleWidth/2;
        float handleY = (slider.y) - handleHeight/2 + slider.height*0.4;

        // Check if the mouse pointer position is inside the bounds of the handle (this line was too long to fit on the screen all at once)
        slider.handleHighlighted = ((int)(logical_x - cursor_hotspot.x-5) >= handleX && (int)(logical_x - cursor_hotspot.x+5) <= handleX + handleWidth &&
                          (int)(logical_y - cursor_hotspot.y-5) >= handleY && (int)(logical_y - cursor_hotspot.y+5) <= handleY + handleHeight);

        if (slider.handleHighlighted && IsMouseButtonPressed(1)) { // Clicking on the slider handle
            slider.handleGrabbed = true;
        } else if (slider.handleGrabbed && IsMouseButtonPressed(1)) { // Stay grabbed if the mouse button is held down and dragged
            slider.handleGrabbed = true;
        } else { // Release the handle, the slider is not in use anymore
            slider.handleGrabbed = false;
        }

        if (slider.handleGrabbed) { // If the slider handle is grabbed, update the slider handle to move with the mouse on its X axis, changing the slider's value
            normalizedSliderValue = logical_x - slider.x;
            slider.value = (normalizedSliderValue / slider.width) * slider.maxValue;
            if (showSettings) { // If the settings panel is showing, the slider is the volume slider, so the apply button is active now that a setting has changed
                settingsApplied = false;
                aSettingHasChanged = true;
                buttons[9].active = true;
            }
        }
    }
}