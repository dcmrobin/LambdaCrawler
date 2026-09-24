#include "Map.h"

int mapWidth = 100; // tiles wide
int mapHeight = 100; // tiles tall
int tileSize = 16;  // pixels per tile 

// Define the mapTiles vector
std::vector<Tile> mapTiles;
Partition map = {mapWidth, mapHeight, 0, 0};
std::vector<Partition> mapPartitions;

int currentLevel = 0;

void InitializeRoom() {
    for (int y = 0; y < 15; ++y) {
        for (int x = 0; x < 20; ++x) {
            Tile tile;
            tile.x = x * tileSize;
            tile.y = y * tileSize;
            tile.hitbox.x = tile.x;
            tile.hitbox.y = tile.y;
            tile.width = tileSize;
            tile.height = tileSize;
            tile.hitbox.width = tile.width;
            tile.hitbox.height = tile.height;
            tile.solid = false;

            if (x == 0 || x == 19 || y == 0 || y == 14) {
                tile.type = x > 0 && y == 0 && x != 19 ? WALL : WALL_TOP;
                tile.solid = true;
            } else if (x == 10 && y == 5) {
                tile.type = CHUTE_CLOSED;
            } else {
                tile.type = GROUND;
            }

            mapTiles.push_back(tile);
        }
    }
}

void GenerateMap() {
    // Fill the map with solid tiles to carve rooms and corridors out of
    for (auto& tile : mapTiles) {
        tile.solid = true;
    }

    // Split the map into partitions until each partition is small enough for only one room or max partitions have been made
    Split(map);
}

void Split(Partition region) {
    // Initialize random utilities for map generation
    std::random_device mapRandomDevice;
    std::mt19937 gen(rd());
    std::bernoulli_distribution coin_flip(0.5);

    // Pick a random axis to split on
    if (coin_flip(gen)) {
        // Horizontal split line
        region.height / 2;
        mapPartitions.push_back()
    } else {
        // Vertical split line
        region.width / 2;
    }
}

void UpdateMap() {
    for (auto& tile : mapTiles) {
        // Update the hitbox based on the tile's position and solid state
        if (tile.solid) {
            tile.hitbox = {tile.x, tile.y, tile.width, tile.height};
        } else {
            tile.hitbox = {0, 0, 0, 0}; // Non-solid tiles have no hitbox
        }
    }
}