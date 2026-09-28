#pragma once
#include "HelperFunctions.h"

#define MAX_PARTITIONS 40

extern int mapWidth;
extern int mapHeight;
extern int tileSize;

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;
extern Partition map;

extern int currentLevel;

void InitializeRoom();
void GenerateMap();
void Split(Partition region);
void UpdateMap();