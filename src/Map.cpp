#include "Map.h"
#include "GFX.h"

#include <vector>

int mapWidth = 20; // tiles wide
int mapHeight = 15; // tiles tall
int tileSize = 16;  // pixels per tile

// Define the mapTiles vector
std::vector<Tile> mapTiles;

int currentLevel = 0;

void InitializeRoom() {
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
    extern int cameraX, cameraY;
    for (const auto& tile : mapTiles) {
        int drawX = tile.x - cameraX;
        int drawY = tile.y - cameraY;
        switch (tile.type)
        {
        case GROUND:
            DrawSprite("tile_ground", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        case WALL:
            DrawSprite("tile_wall", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        case WALL_TOP:
            DrawSprite("tile_wall_top", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        default:
            DrawSprite("tile", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        }
    }
}