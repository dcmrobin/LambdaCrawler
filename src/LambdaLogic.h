#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <set>
#include "GFX.h"

struct LambdaNode {
    enum Type { ABS, APP, VAR } type;
    char var;
    std::shared_ptr<LambdaNode> left, right;
};

// Grid cell
struct GridCell {
    int gridX, gridY;
    bool occupied = false;
    std::shared_ptr<LambdaNode> node = nullptr;
};

using Grid = std::vector<std::vector<GridCell>>;

std::set<char> GetFreeVariables(const std::shared_ptr<LambdaNode>& node);
std::set<char> GetBoundVariables(const std::shared_ptr<LambdaNode>& node);
char GenerateFreshVariable(const std::set<char>& usedVariables);
std::shared_ptr<LambdaNode> AlphaRename(const std::shared_ptr<LambdaNode>& node, char oldVar, char newVar);
std::shared_ptr<LambdaNode> Substitute(const std::shared_ptr<LambdaNode>& node, char var, const std::shared_ptr<LambdaNode>& replacement);
std::string LambdaToString(const std::shared_ptr<LambdaNode>& node);
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth = 3);
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node);
void RenderLambdaGrid(const std::shared_ptr<LambdaNode>& root, int startX, int startY, int cols, int rows, int cellW, int cellH);
std::optional<std::pair<int, int>> FindFreeNeighbor(Grid& grid, int x, int y, int cols, int rows);
void DrawOrthogonalConnection(int x1, int y1, int x2, int y2, SDL_Color color, const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2);
