#pragma once
#include <vector>
#include <string>
#include <memory>
#include "GFX.h" // Assuming SDL drawing functions

struct LambdaNode {
    enum Type { ABS, APP, VAR } type;
    char var;
    std::shared_ptr<LambdaNode> left, right;
};

// Generation
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth = 3);

// Reduction
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node);

// Circuit-style rendering
void RenderLambdaCircuit(const std::shared_ptr<LambdaNode>& node, 
                        int x, int y, 
                        int width, int height);