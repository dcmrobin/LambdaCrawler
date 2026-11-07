#include "Player.h"

Player player = {};

Tile currentIntersectingTile = {ERR, {0, 0, 0, 0}, false, 0, 0};

void HandlePlayerInput() {
    // Handle movement
    auto canMove = [](int newX, int newY) {
        Hitbox newHitbox = player.getWorldHitbox();
        newHitbox.x = newX + player.hitbox.x;
        newHitbox.y = newY + player.hitbox.y;
        int startX = std::max(0, newHitbox.x / tileSize - 1);
        int endX = std::min(mapWidth - 1, (newHitbox.x + newHitbox.width) / tileSize + 1);
        int startY = std::max(0, newHitbox.y / tileSize - 1);
        int endY = std::min(mapHeight - 1, (newHitbox.y + newHitbox.height) / tileSize + 1);
        for (int x = startX; x < endX + 1; x++) {
            for (int y = startY; y < endY + 1; y++) {
                if (mapTiles[x][y].solid && Intersects(newHitbox, mapTiles[x][y].hitbox)) {
                    currentIntersectingTile = mapTiles[x][y];
                    return false;
                }
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
        player.dy = 0;
        player.moving = true;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }
    if (IsKeyPressed(KEY_RIGHT, true)) {
        int newX = player.x + player.speed;
        player.dx = 1;
        player.dy = 0;
        player.moving = true;
        if (canMove(newX, player.y)) {
            player.x = newX;
        }
    }
    if (IsKeyPressed(KEY_RIGHT, true) && IsKeyPressed(KEY_UP, true)) { player.dy = -1; player.dx = 1;}
    if (IsKeyPressed(KEY_RIGHT, true) && IsKeyPressed(KEY_DOWN, true)) { player.dy = 1; player.dx = 1;}
    if (IsKeyPressed(KEY_LEFT, true) && IsKeyPressed(KEY_UP, true)) { player.dy = -1; player.dx = -1;}
    if (IsKeyPressed(KEY_LEFT, true) && IsKeyPressed(KEY_DOWN, true)) { player.dy = 1; player.dx = -1;}
    if (!IsKeyPressed(KEY_UP, true) && !IsKeyPressed(KEY_DOWN, true) && !IsKeyPressed(KEY_LEFT, true) && !IsKeyPressed(KEY_RIGHT, true)) {
        player.moving = false;
        player.dx = 0;
        player.dy = 0;
    }

    const int interactionRange = 24; // pixels
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            // Calculate distance between player and tile
            int dx = (mapTiles[x][y].x + mapTiles[x][y].hitbox.x/2) - (player.x + player.hitbox.x/2);
            int dy = (mapTiles[x][y].y + mapTiles[x][y].hitbox.y/2) - (player.y + player.hitbox.y/2);
            int distanceSq = dx*dx + dy*dy;
            
            if (distanceSq <= interactionRange*interactionRange) {
                if (mapTiles[x][y].type == CHUTE_CLOSED || mapTiles[x][y].type == CHUTE_OPEN) {
                    DrawText("8bitMageFont", mapTiles[x][y].type == CHUTE_CLOSED ? "[X] Open chute" : "[X] Close chute", player.x - 12, player.y - 8, {255, 255, 255, 255}, 0.2f, ALIGN_CENTER);
                    if (mapTiles[x][y].type == CHUTE_OPEN) {
                        DrawText("8bitMageFont", "[Enter] Enter chute", player.x - 12, player.y - 3, {255, 255, 255, 255}, 0.2f, ALIGN_CENTER);
                        if (IsKeyPressed(KEY_ENTER, false)) {
                            // player has gone down the chute
                        }
                    }
                    if (IsKeyPressed(KEY_X, false)) {
                        // Toggle the chute state
                        TileType newType = (mapTiles[x][y].type == CHUTE_CLOSED) ? CHUTE_OPEN : CHUTE_CLOSED;
                        ChangeTile(mapTiles[x][y].x, mapTiles[x][y].y, false, newType);
                        break; // Only interact with one chute at a time
                    }
                }
            }
        }
    }

    if (IsKeyPressed(KEY_Z, false)) {
        // Perform action Z
    }
}