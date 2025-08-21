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

enum class ReductionStrategy {
    NORMAL_ORDER,      // Leftmost, outermost redex first
    APPLICATIVE_ORDER, // Leftmost, innermost redex first  
    CALL_BY_NAME,      // Like normal order but don't reduce inside abstractions
    CALL_BY_NEED       // Call-by-name with memoization (more complex)
};

using Grid = std::vector<std::vector<GridCell>>;

std::set<char> GetFreeVariables(const std::shared_ptr<LambdaNode>& node);
std::set<char> GetBoundVariables(const std::shared_ptr<LambdaNode>& node);
char GenerateFreshVariable(const std::set<char>& usedVariables);
std::shared_ptr<LambdaNode> AlphaRename(const std::shared_ptr<LambdaNode>& node, char oldVar, char newVar);
std::shared_ptr<LambdaNode> Substitute(const std::shared_ptr<LambdaNode>& node, char var, const std::shared_ptr<LambdaNode>& replacement);
std::string LambdaToString(const std::shared_ptr<LambdaNode>& node);
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth = 3);
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node, ReductionStrategy strategy = ReductionStrategy::NORMAL_ORDER);
void RenderLambdaGrid(const std::shared_ptr<LambdaNode>& root, int startX, int startY, int cols, int rows, int cellW, int cellH);
std::optional<std::pair<int, int>> FindFreeNeighbor(Grid& grid, int x, int y, int cols, int rows);
void DrawOrthogonalConnection(int x1, int y1, int x2, int y2, SDL_Color color, const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2);
std::shared_ptr<LambdaNode> NormalOrderReduce(const std::shared_ptr<LambdaNode>& node);
std::shared_ptr<LambdaNode> ApplicativeOrderReduce(const std::shared_ptr<LambdaNode>& node);
std::shared_ptr<LambdaNode> CallByNameReduce(const std::shared_ptr<LambdaNode>& node);
bool HasRedex(const std::shared_ptr<LambdaNode>& node);
