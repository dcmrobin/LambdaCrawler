#include "Player.h"
#include "Map.h"

Player player = {};

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
                return false;
            }
        }
        return true;
    };

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

    // Handle actions
    if (IsKeyPressed(KEY_Z, false)) {
        // Perform action Z
    }
    if (IsKeyPressed(KEY_X, false)) {
        // Perform action X
    }
}