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
                tile.type = x > 0 && y == 0 && x != 19 ? WALL : WALL_TOP;
                mapTiles.push_back(tile);
            }
        }
    }
}

void RenderMap() {
    for (const auto& tile : mapTiles) {
        switch (tile.type)
        {
        case GROUND:
            DrawSprite("tile_ground", tile.x, tile.y, tile.hitbox.width, tile.hitbox.height);
            break;
        case WALL:
            DrawSprite("tile_wall", tile.x, tile.y, tile.hitbox.width, tile.hitbox.height);
            break;
        case WALL_TOP:
            DrawSprite("tile_wall_top", tile.x, tile.y, tile.hitbox.width, tile.hitbox.height);
            break;
        default:
            DrawSprite("tile", tile.x, tile.y, tile.hitbox.width, tile.hitbox.height);
            break;
        }
    }
}