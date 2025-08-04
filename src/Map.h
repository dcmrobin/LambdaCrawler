#pragma once

#include "HelperFunctions.h"

enum TileType {
    EMPTY,
    WALL,
    // Add more tile types as needed
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

void InitializeMapTiles();
void RenderMap();