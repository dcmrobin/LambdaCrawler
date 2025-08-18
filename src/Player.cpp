#include "Player.h"
#include "Map.h"

Player player = {};

Tile currentIntersectingTile = {ERR, {0, 0, 0, 0}, false, 0, 0};

int hopTimer = 0;
void RenderPlayer() {
    if (player.dy == -1 && player.spriteName != "player_idle_back_hop") {
        player.spriteName = "player_idle_back";
    } else if (player.dy == 1 && player.spriteName != "player_idle_hop") {
        player.spriteName = "player_idle";
    }

    if (player.moving) {
        hopTimer++;
        if (hopTimer == 2) {
            player.spriteName = player.spriteName == "player_idle_back" ? "player_idle_back_hop" : player.spriteName == "player_idle" ? "player_idle_hop" : player.spriteName;
        }
        if (hopTimer > 12) {
            player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName;
        }
        if (hopTimer > 22) {
            hopTimer = 0;
        }
    } else {
        player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName;
        hopTimer = 0;
    }

    extern int cameraX, cameraY;
    DrawSprite(player.spriteName, player.x - cameraX, player.y - cameraY, 16, 16);
}

void HandlePlayerInput() {
    // Handle movement
    auto canMove = [](int newX, int newY) {
        Hitbox newHitbox = player.getWorldHitbox();
        newHitbox.x = newX + player.hitbox.x;
        newHitbox.y = newY + player.hitbox.y;
        for (const auto& tile : mapTiles) {
            if (Intersects(newHitbox, tile.hitbox)) {
                currentIntersectingTile = tile;
                return false;
            }
        }
        return true;
    };

    // Handle player movement
    if (IsKeyPressed(KEY_UP, true)) {
        int newY = player.y - player.speed;
        player.dy = -1;
        player.moving = true;
        if (canMove(player.x, newY)) {
            player.y = newY;
        }
    }
    if (IsKeyPressed(KEY_DOWN, true)) {
        int newY = player.y + player.speed;
        player.dy = 1;
        player.moving = true;
        if (canMove(player.x, newY)) {
            player.y = newY;
        }
    }
    if (IsKeyPressed(KEY_LEFT, true)) {
        int newX = player.x - player.speed;
        player.dx = -1;
        player.moving = true;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }
    if (IsKeyPressed(KEY_RIGHT, true)) {
        int newX = player.x + player.speed;
        player.dx = 1;
        player.moving = true;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }
    if (!IsKeyPressed(KEY_UP, true) && !IsKeyPressed(KEY_DOWN, true) && !IsKeyPressed(KEY_LEFT, true) && !IsKeyPressed(KEY_RIGHT, true)) {
        player.moving = false;
        player.dx = 0;
        player.dy = 0;
    }

    const int interactionRange = 24; // pixels
    for (auto& tile : mapTiles) {
        // Calculate distance between player and tile
        int dx = (tile.x + tile.hitbox.x/2) - (player.x + player.hitbox.x/2);
        int dy = (tile.y + tile.hitbox.y/2) - (player.y + player.hitbox.y/2);
        int distanceSq = dx*dx + dy*dy;
        
        if (distanceSq <= interactionRange*interactionRange) {
            if (tile.type == CHUTE_CLOSED || tile.type == CHUTE_OPEN) {
                DrawText("8bitMageFont", tile.type == CHUTE_CLOSED ? "[X] Open chute" : "[X] Close chute", player.x - 12, player.y - 6, {255, 255, 255, 255}, 0.2f);
                if (IsKeyPressed(KEY_X, false)) {
                    // Toggle the chute state
                    TileType newType = (tile.type == CHUTE_CLOSED) ? CHUTE_OPEN : CHUTE_CLOSED;
                    ChangeTile(tile.x, tile.y, false, newType);
                    break; // Only interact with one chute at a time
                }
            }
        }
    }

    if (IsKeyPressed(KEY_Z, false)) {
        // Perform action Z
    }
}