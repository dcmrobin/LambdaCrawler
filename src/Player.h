#pragma once

#include "HelperFunctions.h"
#include "GFX.h"

struct Player {
    int x = 0;
    int y = 0;
    std::string spriteName = "player_idle";
    int speed = 1;
    int maxHealth = 100;
    int currentHealth = 100;
};

extern Player player;

void HandlePlayerInput();