#pragma once

#include "HelperFunctions.h"
#include "GFX.h"

struct Player {
    int x = 40;
    int y = 40;
    std::string spriteName = "player_idle";
    int speed = 1;
    int maxHealth = 100;
    int currentHealth = 100;
    Hitbox hitbox = {3, 1, 10, 15};
    // Returns the hitbox in world coordinates
    Hitbox getWorldHitbox() const {
        return Hitbox{x + hitbox.x, y + hitbox.y, hitbox.width, hitbox.height};
    }
};

extern Player player;

void HandlePlayerInput();