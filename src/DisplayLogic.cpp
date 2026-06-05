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

    }
}

void RenderButtons() {
    for (auto& button : buttons) {
        if (button.text == "null") continue; // Skip uninitialized buttons
        DrawSprite(button.spriteName, button.x, button.y, button.width, button.height); // Draw the button sprite at the button's position

        // Draw the button's text centered on the button
        int centerX = button.x + (button.width / 2);
        int centerY = button.y + (button.height / 2);
        float scale = (LOGICAL_WIDTH * LOGICAL_HEIGHT / 100000.0f) - 0.4f;
        DrawText("8bitMageFont", button.text, centerX, button.pressed || !button.active ? centerY + 1 : centerY, button.textColor, scale, ALIGN_CENTER);
    }
}