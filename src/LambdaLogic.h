#pragma once
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include "GFX.h"

struct LambdaNode {
    enum Type { ABS, APP, VAR } type;
    char var;
    std::shared_ptr<LambdaNode> left, right;
};

std::string LambdaToString(const std::shared_ptr<LambdaNode>& node);
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth = 3);
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node);
void RenderLambdaGrid(const std::shared_ptr<LambdaNode>& root, int startX, int startY, int cols, int rows, int cellW, int cellH);
void DrawConnection(int x1, int y1, int x2, int y2, SDL_Color color);
