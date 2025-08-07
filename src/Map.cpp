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
            Tile tile;
            tile.x = x * tileSize;
            tile.y = y * tileSize;
            tile.hitbox.x = tile.x;
            tile.hitbox.y = tile.y;
            tile.hitbox.width = tileSize;
            tile.hitbox.height = tileSize;
            tile.solid = false;

            if (x == 0 || x == 19 || y == 0 || y == 14) {
                tile.type = x > 0 && y == 0 && x != 19 ? WALL : WALL_TOP;
                tile.solid = true;
            } else if (x == 10 && y == 5) {
                tile.type = CHUTE_CLOSED;
                tile.solid = true;
            } else {
                tile.type = GROUND;
            }

            if (!tile.solid) {
                tile.hitbox = {0, 0, 0, 0};
            }

            mapTiles.push_back(tile);
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
        case CHUTE_CLOSED:
            DrawSprite("tile_chute_closed", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        case CHUTE_OPEN:
            DrawSprite("tile_chute_open", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        default:
            DrawSprite("tile", drawX, drawY, tile.hitbox.width, tile.hitbox.height);
            break;
        }
    }
}