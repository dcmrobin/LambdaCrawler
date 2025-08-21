#include "Map.h"
#include "GFX.h"
#include "Player.h"
#include <queue>
#include <unordered_map>
#include <functional>
#include <vector>

int mapWidth = 20; // tiles wide
int mapHeight = 15; // tiles tall
int tileSize = 16;  // pixels per tile

// Define the mapTiles vector
std::vector<Tile> mapTiles;

int currentLevel = 0;

using Grid = std::vector<std::vector<GridCell>>;

Grid GetLambdaGrid(const std::shared_ptr<LambdaNode>& root, int cols, int rows) {
    Grid grid(rows, std::vector<GridCell>(cols));
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            grid[y][x] = {x, y, false, nullptr};

    std::queue<std::tuple<std::shared_ptr<LambdaNode>, int, int>> q;
    int cx = cols / 2, cy = rows / 2;

    grid[cy][cx].occupied = true;
    grid[cy][cx].node = root;
    q.push({root, cx, cy});

    while (!q.empty()) {
        auto [node, gx, gy] = q.front();
        q.pop();

        auto connect = [&](std::shared_ptr<LambdaNode> child) {
            if (!child) return;
            auto pos = FindFreeNeighbor(grid, gx, gy, cols, rows);
            if (!pos) return;

            int nx = pos->first, ny = pos->second;
            grid[ny][nx].occupied = true;
            grid[ny][nx].node = child;
            q.push({child, nx, ny});
        };

        connect(node->left);
        connect(node->right);
    }

    return grid;
}

// Helper function to determine connection style based on lambda nodes (same as DrawOrthogonalConnection)
bool ShouldConnectHorizontallyFirst(const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2) {
    size_t hash = 0;
    auto hash_combine = [](size_t& seed, size_t value) {
        seed ^= value + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    };
    
    if (node1) {
        hash_combine(hash, std::hash<int>{}(static_cast<int>(node1->type)));
        if (node1->type == LambdaNode::VAR) hash_combine(hash, std::hash<char>{}(node1->var));
    }
    if (node2) {
        hash_combine(hash, std::hash<int>{}(static_cast<int>(node2->type)));
        if (node2->type == LambdaNode::VAR) hash_combine(hash, std::hash<char>{}(node2->var));
    }
    
    return (hash % 2 == 0);
}

void GenerateDungeonFromLambdaGrid(const std::shared_ptr<LambdaNode>& root, int gridCols, int gridRows, int roomSize) {
    bool spawnedPlayer = false;
    if (!root) return;

    mapWidth = roomSize * gridCols;
    mapHeight = roomSize * gridRows;
    
    mapTiles.clear();
    
    // Get the grid layout from the lambda expression
    Grid lambdaGrid = GetLambdaGrid(root, gridCols, gridRows);
    
    // Create rooms at occupied grid positions
    for (int gridY = 0; gridY < gridRows; gridY++) {
        for (int gridX = 0; gridX < gridCols; gridX++) {
            if (lambdaGrid[gridY][gridX].occupied) {
                // Create a room
                int roomX = gridX * roomSize;
                int roomY = gridY * roomSize;
                
                for (int y = 0; y < roomSize; y++) {
                    for (int x = 0; x < roomSize; x++) {
                        Tile tile;
                        tile.x = (roomX + x) * tileSize;
                        tile.y = (roomY + y) * tileSize;
                        tile.width = tileSize;
                        tile.height = tileSize;
                        tile.hitbox = {tile.x, tile.y, tileSize, tileSize};
                        
                        // Create walls on the perimeter, ground inside
                        if (x == 0 || x == roomSize-1 || y == 0 || y == roomSize-1) {
                            tile.type = WALL;
                            tile.solid = true;
                        } else {
                            if (!spawnedPlayer) {
                                player.x = tile.x + tileSize / 2 - player.hitbox.width / 2;
                                player.y = tile.y + tileSize / 2 - player.hitbox.height / 2;
                                spawnedPlayer = true;
                            }
                            tile.type = GROUND;
                            tile.solid = false;
                        }
                        
                        mapTiles.push_back(tile);
                    }
                }
            }
        }
    }
    
    // Create corridors between connected nodes using the same logic as the diagram
    for (int gridY = 0; gridY < gridRows; gridY++) {
        for (int gridX = 0; gridX < gridCols; gridX++) {
            if (lambdaGrid[gridY][gridX].occupied) {
                auto node = lambdaGrid[gridY][gridX].node;
                
                // Check connections to children
                if (node->left) {
                    // Find the child's position in the grid
                    for (int y = 0; y < gridRows; y++) {
                        for (int x = 0; x < gridCols; x++) {
                            if (lambdaGrid[y][x].node == node->left) {
                                CreateOrthogonalCorridor(
                                    gridX * roomSize, gridY * roomSize, 
                                    x * roomSize, y * roomSize, 
                                    roomSize, 
                                    node, node->left
                                );
                                break;
                            }
                        }
                    }
                }
                
                if (node->right) {
                    // Find the child's position in the grid
                    for (int y = 0; y < gridRows; y++) {
                        for (int x = 0; x < gridCols; x++) {
                            if (lambdaGrid[y][x].node == node->right) {
                                CreateOrthogonalCorridor(
                                    gridX * roomSize, gridY * roomSize, 
                                    x * roomSize, y * roomSize, 
                                    roomSize, 
                                    node, node->right
                                );
                                break;
                            }
                        }
                    }
                }
            }
        }
    }

    CleanUpMapTiles();
}

void CleanUpMapTiles() {
    // Create a 2D grid representation for easier neighbor checking
    std::vector<std::vector<Tile*>> grid(mapHeight, std::vector<Tile*>(mapWidth, nullptr));
    
    // Populate the grid
    for (auto& tile : mapTiles) {
        int gridX = tile.x / tileSize;
        int gridY = tile.y / tileSize;
        if (gridX >= 0 && gridX < mapWidth && gridY >= 0 && gridY < mapHeight) {
            grid[gridY][gridX] = &tile;
        }
    }
    
    // First pass: Add missing tiles around ground tiles
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (grid[y][x] == nullptr) continue;
            
            Tile& tile = *grid[y][x];
            
            if (tile.type == GROUND) {
                // Check above
                if (y > 0 && grid[y-1][x] == nullptr) {
                    Tile newTile;
                    newTile.type = WALL;
                    newTile.solid = true;
                    newTile.x = x * tileSize;
                    newTile.y = (y-1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y-1][x] = &mapTiles.back();
                }
                
                // Check left and right
                if (x > 0 && (grid[y][x-1] == nullptr || (grid[y][x-1] != nullptr && grid[y][x-1]->type != WALL_TOP && grid[y][x-1]->type != GROUND))) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x-1) * tileSize;
                    newTile.y = y * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y][x-1] = &mapTiles.back();
                }
                
                if (x < mapWidth-1 && (grid[y][x+1] == nullptr || (grid[y][x+1] != nullptr && grid[y][x+1]->type != WALL_TOP && grid[y][x+1]->type != GROUND))) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x+1) * tileSize;
                    newTile.y = y * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y][x+1] = &mapTiles.back();
                }
                
                // Check below
                if (y < mapHeight-1 && (grid[y+1][x] == nullptr || (grid[y+1][x] != nullptr && grid[y+1][x]->type != WALL && grid[y+1][x]->type != WALL_TOP && grid[y+1][x]->type != GROUND))) {
                    Tile newTile;
                    newTile.type = WALL;
                    newTile.solid = true;
                    newTile.x = x * tileSize;
                    newTile.y = (y+1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y+1][x] = &mapTiles.back();
                }

                // Check corners
                if (x > 0 && y > 0 && grid[y-1][x-1] == nullptr) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x-1) * tileSize;
                    newTile.y = (y-1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y-1][x-1] = &mapTiles.back();
                }

                if (x < mapWidth-1 && y > 0 && grid[y-1][x+1] == nullptr) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x+1) * tileSize;
                    newTile.y = (y-1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y-1][x+1] = &mapTiles.back();
                }

                if (x > 0 && y < mapHeight-1 && grid[y+1][x-1] == nullptr) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x-1) * tileSize;
                    newTile.y = (y+1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y+1][x-1] = &mapTiles.back();
                }

                if (x < mapWidth-1 && y < mapHeight-1 && grid[y+1][x+1] == nullptr) {
                    Tile newTile;
                    newTile.type = WALL_TOP;
                    newTile.solid = true;
                    newTile.x = (x+1) * tileSize;
                    newTile.y = (y+1) * tileSize;
                    newTile.width = tileSize;
                    newTile.height = tileSize;
                    newTile.hitbox = {newTile.x, newTile.y, tileSize, tileSize};
                    mapTiles.push_back(newTile);
                    grid[y+1][x+1] = &mapTiles.back();
                }
            }
        }
    }
    
    // Second pass: Adjust wall types based on neighboring tiles
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (grid[y][x] == nullptr) continue;
            
            Tile& tile = *grid[y][x];

            if (tile.type == WALL_TOP) {
                if (y < mapHeight-1 && grid[y+1][x] != nullptr && grid[y+1][x]->type == GROUND) {
                    tile.type = WALL;
                }
            } else if (tile.type == WALL) {
                if (y < mapHeight-1 && grid[y+1][x] != nullptr && (grid[y+1][x]->type == WALL || grid[y+1][x]->type == WALL_TOP)) {
                    tile.type = WALL_TOP;
                }
            }
        }
    }

    // Final pass: Turn walltop tiles that are above empty tiles into walls
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (grid[y][x] == nullptr) continue;
            
            Tile& tile = *grid[y][x];

            if (tile.type == WALL_TOP) {
                if (grid[y+1][x] == nullptr) {
                    tile.type = WALL;
                }
            }
        }
    }
}

void CreateOrthogonalCorridor(int startGridX, int startGridY, int endGridX, int endGridY, int roomSize, const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2) {
    // Convert room grid coordinates to tile coordinates
    int startX = startGridX + roomSize / 2;
    int startY = startGridY + roomSize / 2;
    int endX = endGridX + roomSize / 2;
    int endY = endGridY + roomSize / 2;
    
    // Use the same logic as DrawOrthogonalConnection to determine connection style
    bool horizontalFirst = ShouldConnectHorizontallyFirst(node1, node2);
    
    if (horizontalFirst) {
        // Horizontal then vertical
        for (int x = std::min(startX, endX); x <= std::max(startX, endX); x++) {
            AddCorridorTile(x, startY, roomSize);
        }
        for (int y = std::min(startY, endY); y <= std::max(startY, endY); y++) {
            AddCorridorTile(endX, y, roomSize);
        }
    } else {
        // Vertical then horizontal
        for (int y = std::min(startY, endY); y <= std::max(startY, endY); y++) {
            AddCorridorTile(startX, y, roomSize);
        }
        for (int x = std::min(startX, endX); x <= std::max(startX, endX); x++) {
            AddCorridorTile(x, endY, roomSize);
        }
    }
}

void AddCorridorTile(int gridX, int gridY, int roomSize) {
    int tileX = gridX * tileSize;
    int tileY = gridY * tileSize;
    
    // Check if a tile already exists at this position
    for (auto& tile : mapTiles) {
        if (tile.x == tileX && tile.y == tileY) {
            // Convert wall to ground if it's a wall
            if (tile.type == WALL) {
                tile.type = GROUND;
                tile.solid = false;
                tile.hitbox = {0, 0, 0, 0};
            }
            return;
        }
    }
    
    // Add a new corridor tile
    Tile tile;
    tile.x = tileX;
    tile.y = tileY;
    tile.width = tileSize;
    tile.height = tileSize;
    tile.type = GROUND;
    tile.solid = false;
    tile.hitbox = {0, 0, 0, 0};
    mapTiles.push_back(tile);
}

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

void RenderMap() {
    extern int cameraX, cameraY;
    for (const auto& tile : mapTiles) {
        int drawX = tile.x - cameraX;
        int drawY = tile.y - cameraY;
        switch (tile.type)
        {
        case GROUND:
            DrawSprite("tile_ground", drawX, drawY, tile.width, tile.height);
            break;
        case WALL:
            DrawSprite("tile_wall", drawX, drawY, tile.width, tile.height);
            break;
        case WALL_TOP:
            DrawSprite("tile_wall_top", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_CLOSED:
            DrawSprite("tile_chute_closed", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_OPEN:
            DrawSprite("tile_chute_open", drawX, drawY, tile.width, tile.height);
            break;
        default:
            DrawSprite("tile", drawX, drawY, tile.width, tile.height);
            break;
        }
    }
}