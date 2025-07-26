#include "Player.h"

Player player = {};

void HandlePlayerInput() {
    // Handle movement
    if (IsKeyPressed(KEY_UP, true)) {
        player.y -= player.speed;
    }
    if (IsKeyPressed(KEY_DOWN, true)) {
        player.y += player.speed;
    }
    if (IsKeyPressed(KEY_LEFT, true)) {
        player.x -= player.speed;
    }
    if (IsKeyPressed(KEY_RIGHT, true)) {
        player.x += player.speed;
    }

    // Handle actions
    if (IsKeyPressed(KEY_Z, false)) {
        // Perform action Z
    }
    if (IsKeyPressed(KEY_X, false)) {
        // Perform action X
    }
}