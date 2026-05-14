#pragma once
#include <vector>
#include <string>
#include "HelperFunctions.h"

extern int mapWidth;
extern int mapHeight;
extern int tileSize;

// The map is a collection of tiles
extern std::vector<Tile> mapTiles;

extern int currentLevel;

Grid GetLambdaGrid(const std::shared_ptr<LambdaNode>& root, int cols, int rows);
bool ShouldConnectHorizontallyFirst(const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2);
void GenerateDungeonFromLambdaGrid(const std::shared_ptr<LambdaNode>& root, int gridCols, int gridRows, int roomSize);
void CleanUpMapTiles();
void CreateOrthogonalCorridor(int startGridX, int startGridY, int endGridX, int endGridY, int roomSize, const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2);
void AddCorridorTile(int gridX, int gridY, int roomSize);
void InitializeRoom();
void UpdateMap();
void RenderMap();