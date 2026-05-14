#pragma once
#include "HelperFunctions.h"

extern int mapWidth;
extern int mapHeight;
extern int tileSize;

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;

extern int currentLevel;

void InitializeRoom();
void UpdateMap();