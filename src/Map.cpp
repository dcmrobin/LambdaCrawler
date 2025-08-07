#include "Map.h"
#include "GFX.h"

#include <vector>

// Define the mapTiles vector
std::vector<Tile> mapTiles;

void InitializeRoom() {
    int tileSize = 16;
    for (int y = 0; y < 15; ++y) {
        for (int x = 0; x < 20; ++x) {
            if (x == 0 || x == 19 || y == 0 || y == 14) {
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