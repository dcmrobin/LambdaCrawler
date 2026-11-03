#include "Map.h"

int mapWidth = 100; // tiles wide
int mapHeight = 100; // tiles tall
int tileSize = 16;  // pixels per tile

// map generation constraints
int maxSplits = 200;
int currentSplitCount = 0;
int maxPartitionSize = 20;

// Define the mapTiles vector
std::vector<std::vector<Tile>> mapTiles;
Partition map = {mapWidth, mapHeight, 0, 0};
std::vector<Partition> mapPartitions;

int currentLevel = 0;

void GenerateMap() {
    // Reset mapTiles vector
    mapTiles.assign(mapWidth, std::vector<Tile>(mapHeight));
    mapPartitions.clear();

    // Reset split count just in case the map is being regenerated
    currentSplitCount = 0;

    // Fill the map with solid tiles to carve rooms and corridors out of
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            Tile tile; // Declare new tile to be assigned to the tile at coords x and y
            tile.type = WALL_TOP; // Set type to a tile that would be solid
            tile.solid = true; // Set solid to true so that player cannot physically walk through it
            tile.x = x * tileSize; // Set position to the current coordinates
            tile.y = y * tileSize;
            tile.hitbox.x = tile.x; // Make sure the tile's hitbox is in line with the tile
            tile.hitbox.y = tile.y;
            tile.width = tileSize; // Set tile size
            tile.height = tileSize;
            tile.hitbox.width = tile.width; // Make sure the tile's hitbox is the same size as the tile
            tile.hitbox.height = tile.height;
            mapTiles[x][y] = tile; // Finally, set the tile at the current coordinates inside mapTiles[][] to the new tile that was just declared
        }
    }

    // Split the map into partitions until each partition is small enough for only one room or max partitions have been made
    Split(map, 0);

    // Remove some rooms to create a more open layout
    int roomsToRemove = (int)(mapPartitions.size() / 2); // Always remove half of the rooms generated
    for (int i = 0; i < roomsToRemove; i++) {
        int randIndex = Random(0, (int)mapPartitions.size() - 1); // Find a random room to remove,
        mapPartitions.erase(mapPartitions.begin() + randIndex); // And remove it
    } // Repeat until half the rooms have been removed

    // Carve out all partitions (rooms) in the map of solid tiles
    for (auto& partition : mapPartitions) {
        for (int px = partition.x + 1; px < partition.x + partition.width; px++) {
            for (int py = partition.y + 1; py < partition.y + partition.height; py++) {
                if (px < mapWidth - 1 && py < mapHeight - 1) { // Ensure it doesn't go out of bounds
                    mapTiles[px][py].solid = false;
                    mapTiles[px][py].type = GROUND;
                }
            }
        }
    }

    // Carve corridors between each room
    for (int i = 0; mapPartitions.size() > 1 && i < (int)mapPartitions.size(); i++) {
        int randIndex = Random(0, (int)mapPartitions.size() - 2); // Pick a random partition (room) to connect to
        if (randIndex >= i) {
            randIndex++; // Ensure a room does not connect to itself
        }

        bool alreadyConnected = mapPartitions[i].connected && mapPartitions[randIndex].connected;
        if (alreadyConnected) {
            continue; // If both rooms are already connected, skip to the next iteration to avoid redundant connections
        }

        // Declare both rooms and their centers
        const Partition& currentPartition = mapPartitions[i];
        const Partition& targetPartition = mapPartitions[randIndex];
        const int currentCenterX = currentPartition.x + currentPartition.width / 2;
        const int currentCenterY = currentPartition.y + currentPartition.height / 2;
        const int targetCenterX = targetPartition.x + targetPartition.width / 2;
        const int targetCenterY = targetPartition.y + targetPartition.height / 2;

        // Carve a connected L-shaped corridor between the room centers
        for (int x = std::min(currentCenterX, targetCenterX); x <= std::max(currentCenterX, targetCenterX); x++) { // The horizontal part
            mapTiles[x][currentCenterY].solid = false;
            mapTiles[x][currentCenterY].type = GROUND;
        }
        for (int y = std::min(currentCenterY, targetCenterY); y <= std::max(currentCenterY, targetCenterY); y++) { // The vertical part
            mapTiles[targetCenterX][y].solid = false;
            mapTiles[targetCenterX][y].type = GROUND;
        }

        mapPartitions[i].connected = true;
        mapPartitions[randIndex].connected = true; // Mark both current and target partitions as connected
    }

    // Clean up map- any wall-top tiles that have a ground tile to their south, will be turned into a wall tile, this adds a sense of depth to the walls
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            if (mapTiles[x][y].type == WALL_TOP) { // Only consider wall tiles
                if (y + 1 < mapHeight && mapTiles[x][y + 1].type == GROUND) { // Check if the neighbour tile to the south of this one is a ground tile
                    mapTiles[x][y].type = WALL; // If it is, then change this tile to a wall variant tile, one that acts like a transition between walls and ground
                }
            }
        }
    }
}

// Axis: 0 = random, 1 = horizontal, 2 = vertical
void Split(Partition region, int axis) {
    // Make sure infinite splitting does not occur, also make sure any partitions that are hoping to get split instantly get saved to the list if the max splits have been met
    currentSplitCount++;
    if (currentSplitCount >= maxSplits) {
        mapPartitions.push_back(region); // Max splits have been met, save the the unsplit partition
        return; 
    }

    // Pick a random axis to split on
    bool randAxis = CoinFlip(); // 50% chance to be either horizontal or vertical
    if ((randAxis && axis == 0) || axis == 1) {
        if (region.height > maxPartitionSize) { // Don't split if room is already small enough on that axis
            // Horizontal split line

            int randOffset = Random(-(int)(maxPartitionSize/3), (int)(region.height/3));

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
            if (tp.width > maxPartitionSize || tp.height > maxPartitionSize) {
                Split(tp, 0);
            } else if (tp.width <= maxPartitionSize && tp.height <= maxPartitionSize) {
                mapPartitions.push_back(tp);
            }
            if (bp.width > maxPartitionSize || bp.height > maxPartitionSize) {
                Split(bp, 0);
            } else if (bp.width <= maxPartitionSize && bp.height <= maxPartitionSize) {
                mapPartitions.push_back(bp);
            }
        } else if (region.height <= maxPartitionSize && region.width > maxPartitionSize) {
            Split(region, 2); // Region cannot split horizontally due to being too small on that dimention already, so split vertically (but only if it can be split vertically)
        } else {
            mapPartitions.push_back(region);
            return; // Region cannot be split anymore, save it
        }
    } else if ((!randAxis && axis == 0) || axis == 2) {
        if (region.width > maxPartitionSize) { // Don't split if room is already small enough on that axis
            // Vertical split line
            
            int randOffset = Random(-(int)(maxPartitionSize/3), (int)(region.width/3));

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
            if (lp.width > maxPartitionSize || lp.height > maxPartitionSize) {
                Split(lp, 0);
            } else if (lp.width <= maxPartitionSize && lp.height <= maxPartitionSize) {
                mapPartitions.push_back(lp);
            }
            if (rp.width > maxPartitionSize || rp.height > maxPartitionSize) {
                Split(rp, 0);
            } else if (rp.width <= maxPartitionSize && rp.height <= maxPartitionSize) {
                mapPartitions.push_back(rp);
            }
        } else if (region.width <= maxPartitionSize && region.height > maxPartitionSize) {
            Split(region, 1); // Region cannot split vertically due to being too small on that dimention already, so split horizontally (but only if it can be split horizontally)
        } else {
            mapPartitions.push_back(region);
            return; // Region cannot be split anymore, save it
        }
    }
}

void UpdateMap() {
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            // Update the hitbox based on the tile's position and solid state
            if (mapTiles[x][y].solid) {
                mapTiles[x][y].hitbox = {mapTiles[x][y].x, mapTiles[x][y].y, mapTiles[x][y].width, mapTiles[x][y].height};
            } else {
                mapTiles[x][y].hitbox = {0, 0, 0, 0}; // Non-solid tiles have no hitbox
            }
        }
    }
}