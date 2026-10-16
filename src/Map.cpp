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
    currentSplitCount = 0;

    // Fill the map with solid tiles to carve rooms and corridors out of
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            Tile tile;
            tile.type = WALL_TOP;
            tile.solid = true;
            tile.x = x * tileSize;
            tile.y = y * tileSize;
            tile.hitbox.x = tile.x;
            tile.hitbox.y = tile.y;
            tile.width = tileSize;
            tile.height = tileSize;
            tile.hitbox.width = tile.width;
            tile.hitbox.height = tile.height;
            mapTiles[x][y] = tile;
        }
    }

    // Split the map into partitions until each partition is small enough for only one room or max partitions have been made
    Split(map, 0);

    // Carve out all partitions in the map of solid tiles
    for (auto& partition : mapPartitions) {
        for (int px = partition.x + 1; px < partition.x + partition.width; px++) {
            for (int py = partition.y + 1; py < partition.y + partition.height; py++) {
                mapTiles[px][py].solid = false;
                mapTiles[px][py].type = GROUND;
            }
        }
    }

    // Carve corridors between each room
    for (int i = 0; i < (int)mapPartitions.size(); i++) {
        int randIndex = Random(0, (int)(mapPartitions.size() - 1));
        if (randIndex == i) { randIndex += ((randIndex + 1) >= (int)mapPartitions.size()) ? -1 : 1; } // Make sure target partition index is not this partition

        Partition targetPartition = mapPartitions[randIndex];
        // Carve horizontal segment of corridor
        if (mapPartitions[i].x > targetPartition.x) {
            // Is target partition to the right of this partition?
            for (int x = (int)((mapPartitions[i].width / 2) + mapPartitions[i].x); x < (targetPartition.width / 2) + targetPartition.x; x++) {
                mapTiles[x][mapPartitions[i].height + mapPartitions[i].y].solid = false;
                mapTiles[x][mapPartitions[i].height + mapPartitions[i].y].type = GROUND;
            }
        } else if (mapPartitions[i].x < targetPartition.x) {
            // Is target partition to the left of this partition?
            for (int x = (int)((mapPartitions[i].width / 2) + mapPartitions[i].x); x > (targetPartition.width / 2) + targetPartition.x; x--) {
                mapTiles[x][mapPartitions[i].height + mapPartitions[i].y].solid = false;
                mapTiles[x][mapPartitions[i].height + mapPartitions[i].y].type = GROUND;
            }
        }
    }

    // Clean up map- any wall-top tiles that have a ground tile to their south, will be turned into a wall tile, this adds verticality
    for (int x = 0; x < mapWidth; x++) {
        for (int y = 0; y < mapHeight; y++) {
            if (mapTiles[x][y].type == WALL_TOP) {
                if (y + 1 < mapHeight && mapTiles[x][y + 1].type == GROUND) {
                    mapTiles[x][y].type = WALL;
                }
            }
        }
    }
}

// Axis: 0 = random, 1 = horizontal, 2 = vertical
void Split(Partition region, int axis) {
    // Make sure infinite splitting does not occur, but also make sure any partitions that are hoping to get split instantly get saved to the list if the max splits have been met
    currentSplitCount++;
    if (currentSplitCount >= maxSplits) {
        mapPartitions.push_back(region);
        return; 
    }

    bool randAxis = CoinFlip();

    // Pick a random axis to split on
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
            return; // Region cannot be split anymore, cache it
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
            return; // Region cannot be split anymore, cache it
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