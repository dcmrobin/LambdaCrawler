#include "Map.h"

int mapWidth = 100; // tiles wide
int mapHeight = 100; // tiles tall
int tileSize = 16;  // pixels per tile

// map generation constraints
int maxSplits = 10;
int currentSplitCount = 0;
int maxRoomSize = 5;

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

    for (auto& part :mapPartitions) {
        SDL_Log("partition x: %d", part.x);
        SDL_Log("partition y: %d", part.y);
        SDL_Log("partition width: %d", part.width);
        SDL_Log("partition height: %d", part.height);
    }
}

void Split(Partition region) {
    // Make sure infinite splitting does not occur, but also make sure any partitions that are hoping to get split instantly get saved to the list if the max splits have been met
    currentSplitCount++;
    if (currentSplitCount >= maxSplits) {
        mapPartitions.push_back(region);
        return; 
    }

    // Pick a random axis to split on
    if (CoinFlip()) {
        // Horizontal split line
        int randOffsetRaw = Random(-20, 20);
        float normalizedT = (randOffsetRaw + 20) / 40.0f;
        int randOffset = (int)(normalizedT * region.height) - (region.height / 2); // Normalize the random offset value to fit inside the room

        Partition tp; // Top partition
        Partition bp; // Bottom partition
        tp.x = region.x;
        tp.y = region.y; 
        tp.width = region.width;
        tp.height = (region.height / 2) + randOffset;
        bp.x = region.x;
        bp.y = region.y + tp.height;
        bp.width = region.width;
        bp.height = region.height - tp.height;

        // Check if partition is small enough for a single room - if not, split some more
        if (tp.width > maxRoomSize || tp.height > maxRoomSize) {
            Split(tp);
        } else if (tp.width <= maxRoomSize && tp.height <= maxRoomSize) {
            mapPartitions.push_back(tp);
        }
        if (bp.width > maxRoomSize || bp.height > maxRoomSize) {
            Split(bp);
        } else if (bp.width <= maxRoomSize && bp.height <= maxRoomSize) {
            mapPartitions.push_back(bp);
        }
    } else {
        // Vertical split line
        int randOffsetRaw = Random(-20, 20);
        float normalizedT = (randOffsetRaw + 20) / 40.0f;
        int randOffset = (int)(normalizedT * region.width) - (region.width / 2); // Normalize the random offset value to fit inside the room

        Partition lp; // Left partition
        Partition rp; // Right partition
        lp.x = region.x;
        lp.y = region.y;
        lp.width = (region.width / 2) + randOffset;
        lp.height = region.height;
        rp.x = region.x + lp.width;
        rp.y = region.y;
        rp.width = region.width - lp.width;
        rp.height = region.height;

        // Check if partition is small enough for a single room - if not, split some more
        if (lp.width > maxRoomSize || lp.height > maxRoomSize) {
            Split(lp);
        } else if (lp.width <= maxRoomSize && lp.height <= maxRoomSize) {
            mapPartitions.push_back(lp);
        }
        if (rp.width > maxRoomSize || rp.height > maxRoomSize) {
            Split(rp);
        } else if (rp.width <= maxRoomSize && rp.height <= maxRoomSize) {
            mapPartitions.push_back(rp);
        }
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