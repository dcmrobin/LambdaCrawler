#include "Map.h"
#include "GFX.h"

#include <vector>

// Define the mapTiles vector
std::vector<Tile> mapTiles;

// Example function to initialize tiles (customize as needed)
void InitializeMapTiles() {
    // Example: create a 10x10 grid of tiles, each 16x16 pixels
    int tileSize = 16;
    for (int y = 4; y < 10; ++y) {
        for (int x = 4; x < 10; ++x) {
            Tile tile;
            tile.x = x * tileSize;
            tile.y = y * tileSize;
            tile.hitbox.x = tile.x;
            tile.hitbox.y = tile.y;
            tile.hitbox.width = tileSize;
            tile.hitbox.height = tileSize;
            tile.type = WALL;
            mapTiles.push_back(tile);
        }
    }
}

void RenderMap() {
    for (const auto& tile : mapTiles) {
        switch (tile.type)
        {
        case EMPTY:
            // nothing for now, implement a floor sprite later
            break;
        case WALL:
            DrawSprite("tile", tile.x, tile.y, tile.hitbox.width, tile.hitbox.height);
            break;
        default:
            break;
        }
    }
}