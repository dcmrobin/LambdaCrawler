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

extern int mapWidth;
extern int mapHeight;
extern int tileSize;

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;

extern int currentLevel;

void InitializeRoom();
void RenderMap();