#include "DisplayLogic.h"

int hopTimer = 0;
void RenderPlayer() {
    if (player.dy == -1 && player.spriteName != "player_idle_back_hop") {
        player.spriteName = "player_idle_back";
    } else if (player.dy == 1 && player.spriteName != "player_idle_hop") {
        player.spriteName = "player_idle";
    }
    if ((player.dx == -1 || player.dx == 1) && (player.dy == 0) && player.spriteName != "player_idle_side_hop") {
        player.spriteName = "player_idle_side";
    }

    if (player.moving) {
        hopTimer++;
        if (hopTimer == 2) {
            player.spriteName = player.spriteName == "player_idle_back" ? "player_idle_back_hop" : player.spriteName == "player_idle" ? "player_idle_hop" : player.spriteName == "player_idle_side_hop" ? "player_idle_side" : player.spriteName;
        }
        if (hopTimer > 12) {
            player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName == "player_idle_side" ? "player_idle_side_hop" : player.spriteName;
        }
        if (hopTimer > 22) {
            hopTimer = 0;
        }
    } else {
        player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName == "player_idle_side_hop" ? "player_idle_side" : player.spriteName;
        hopTimer = 0;
    }

    extern int cameraX, cameraY;
    DrawSprite(player.spriteName, player.x - cameraX, player.y - cameraY, 16, 32);
}

void RenderMap() {
    extern int cameraX, cameraY;
    for (const auto& tile : mapTiles) {
        int drawX = tile.x - cameraX;
        int drawY = tile.y - cameraY;
        switch (tile.type)
        {
        case GROUND:
            DrawSprite("tile_ground", drawX, drawY, tile.width, tile.height);
            break;
        case WALL:
            DrawSprite("tile_wall", drawX, drawY, tile.width, tile.height);
            break;
        case WALL_TOP:
            DrawSprite("tile_wall_top", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_CLOSED:
            DrawSprite("tile_chute_closed", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_OPEN:
            DrawSprite("tile_chute_open", drawX, drawY, tile.width, tile.height);
            break;
        default:
            DrawSprite("tile", drawX, drawY, tile.width, tile.height);
            break;
        }
    }
}

void RenderMenu() {
    DrawSprite("menu_splash", 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT); // Draw the menu splash screen background to the width and height of the viewport
    DrawSprite("title_sprite", 0, 0, 150, 70); // Draw the "LAMBDA CRAWLER" title graphic at the top left of the viewport
    // Moved the RenderButtons call back to the main loop, as nothing will be drawn on top of buttons
    if (showSettings) {
        DrawSprite("button", LOGICAL_WIDTH/2 - 30, LOGICAL_HEIGHT/2 - 130, 100, 240); // Draw the settings panel sprite if showSettings is true (button sprite used for now)
        DrawText("8bitMageFont", "Settings", LOGICAL_WIDTH/2 + 10, LOGICAL_HEIGHT/2 - 50, {63, 63, 63}, 0.5, ALIGN_CENTER); // Draw settings panel header

        // Set up exit settings button
        buttons[4].text = ""; // Exit settings button has no text on it
        // Set the action for the exit settings button to BTN_HIDE_SETTINGS, which sets the showSettings flag to false, hiding the settings panel
        buttons[4].action = BTN_HIDE_SETTINGS;
        // Position the exit settings button at the top right of the settings panel
        buttons[4].x = LOGICAL_WIDTH/2 + 48;
        buttons[4].y = LOGICAL_HEIGHT/2 - 60;
        // Set the exit button's scale to square
        buttons[4].width = 20;
        buttons[4].height = 20;

        // Set up volume slider
        sliders[0].text = "Volume: "; // Set the label for the volume slider
        // Set the volume slider's width and height
        sliders[0].width = 70;
        sliders[0].height = 10;
        // Position volume slider underneath the settings header
        sliders[0].x = LOGICAL_WIDTH/2 - sliders[0].width/2 + 20;
        sliders[0].y = LOGICAL_HEIGHT/2 - 30;

        // Set up window mode options
        DrawText("8bitMageFont", "Window mode:", LOGICAL_WIDTH/2 + 20, LOGICAL_HEIGHT/2 - 15, {63, 63, 63}, 0.3, ALIGN_CENTER); // Window options header
        // Fullscreen button
        buttons[5].text = "Fullscreen";
        buttons[5].textSize = 0.2f;
        buttons[5].action = BTN_FULLSCREEN;
        buttons[5].x = LOGICAL_WIDTH/2 - 20;
        buttons[5].y = LOGICAL_HEIGHT/2 - 15;
        buttons[5].width = 40;
        buttons[5].height = 20;
        // Windowed button
        buttons[6].text = "Windowed";
        buttons[6].textSize = 0.2f;
        buttons[6].action = BTN_WINDOWED;
        buttons[6].x = LOGICAL_WIDTH/2 + 20;
        buttons[6].y = LOGICAL_HEIGHT/2 - 15;
        buttons[6].width = 40;
        buttons[6].height = 20;
    } else {
        sliders[0].text = "null"; // Hide the volume slider when the settings panel is not showing
    }
}

void RenderButtons() {
    for (auto& button : buttons) {
        if (button.text == "null") continue; // Skip uninitialized buttons
        DrawSprite(button.spriteName, button.x, button.y, button.width, button.height); // Draw the button sprite at the button's position

        // Draw the button's text centered on the button
        int centerX = button.x + (button.width / 2);
        int centerY = button.y + (button.height / 2);
        DrawText("8bitMageFont", button.text, centerX, button.pressed || !button.active ? centerY + 1 : centerY, button.textColor, button.textSize, ALIGN_CENTER);
    }
}

void RenderSliders() {
    for (auto& slider : sliders) {
        if (slider.text == "null") continue; // Skip uninitialized sliders

        // Normalize slider's value for drawing the handle, as the width will not always be a clean 100
        float normalizedSliderValue = (slider.value / slider.maxValue) * slider.width;

        // Draw slider label with value printed as well- example: "Volume: 45"
        DrawText("8bitMageFont", slider.text + std::to_string((int)slider.value), slider.x, slider.y - slider.height, slider.textColor, 0.3, ALIGN_LEFT);
        
        FillRect(slider.x, slider.y, slider.width, slider.height*0.8, {0, 0, 0, 255}); // Draw the slider's background with a height of slightly less than the sliders actual height
        FillRect(slider.x + normalizedSliderValue, slider.y - (slider.height*0.2)/2, 6, slider.height, {89, 86, 82, 255}); // Draw the slider's handle's outline
        // Draw the slider's handle in the appropriate colour depending on if it is active, grabbed, or highlighted
        SDL_Color handleColour = {155, 173, 183, 255};
        if (slider.handleHighlighted) {
            handleColour = {203, 219, 252, 255};
        }
        if (slider.handleGrabbed || !slider.handleActive) {
            handleColour = {89, 86, 82, 255};
        }
        FillRect((slider.x + normalizedSliderValue) + (6 * 0.2)/2, (slider.y - (slider.height*0.2)/2) + (slider.height * 0.2)/2, 6 * 0.8, slider.height * 0.8, handleColour);
    }
}