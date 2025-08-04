#include "Player.h"
#include "Map.h"

Player player = {};

void HandlePlayerInput() {
    // Handle movement
    auto canMove = [](int newX, int newY) {
        Hitbox newHitbox = player.getWorldHitbox();
        newHitbox.x = newX + player.hitbox.x;
        newHitbox.y = newY + player.hitbox.y;
        for (const auto& tile : mapTiles) {
            if (Intersects(newHitbox, tile.hitbox)) {
                return false;
            }
        }
        return true;
    };

    if (IsKeyPressed(KEY_UP, true)) {
        int newY = player.y - player.speed;
        if (canMove(player.x, newY)) {
            player.y = newY;
        }
    }
    if (IsKeyPressed(KEY_DOWN, true)) {
        int newY = player.y + player.speed;
        if (canMove(player.x, newY)) {
            player.y = newY;
        }
    }
    if (IsKeyPressed(KEY_LEFT, true)) {
        int newX = player.x - player.speed;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }
    if (IsKeyPressed(KEY_RIGHT, true)) {
        int newX = player.x + player.speed;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }

    // Handle actions
    if (IsKeyPressed(KEY_Z, false)) {
        // Perform action Z
    }
    if (IsKeyPressed(KEY_X, false)) {
        // Perform action X
    }
}