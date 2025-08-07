#pragma once

#include "HelperFunctions.h"

enum TileType {
    ERR,
    GROUND,
    WALL,
    WALL_TOP,
};

// Represents a single tile in the map
struct Tile {
    TileType type;
    Hitbox hitbox;
    int x;
    int y;
};

#include <vector>

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;

void InitializeRoom();
void RenderMap();