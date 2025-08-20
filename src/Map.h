#pragma once
#include <vector>
#include <string>
#include "HelperFunctions.h"
#include "LambdaLogic.h"

extern int mapWidth;
extern int mapHeight;
extern int tileSize;

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;

extern int currentLevel;

Grid GetLambdaGrid(const std::shared_ptr<LambdaNode>& root, int cols, int rows);
void GenerateDungeonFromLambdaGrid(const std::shared_ptr<LambdaNode>& root, int gridCols, int gridRows, int roomSize);
void CreateCorridor(int startGridX, int startGridY, int endGridX, int endGridY, int roomSize);
void AddCorridorTile(int gridX, int gridY, int roomSize);
void InitializeRoom();
void UpdateMap();
void RenderMap();