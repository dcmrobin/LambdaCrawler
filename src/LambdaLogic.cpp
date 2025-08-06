#include "LambdaLogic.h"
#include <random>
#include <stack>

// Circuit colors
const SDL_Color WIRE_COLOR = {100, 255, 100, 255};  // Neon green
const SDL_Color GATE_COLOR = {255, 50, 50, 255};    // Red
const SDL_Color TERMINAL_COLOR = {100, 100, 255, 255}; // Blue
const SDL_Color PORT_COLOR = {255, 255, 100, 255};  // Yellow

// Use ASCII characters
static const char vars[] = {'a', 'b', 'c', 'd'}; // Changed from Greek letters

std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    
    if (maxDepth <= 0) {
        auto node = std::make_shared<LambdaNode>();
        node->type = LambdaNode::VAR;
        node->var = vars[gen() % 4];
        return node;
    }

    switch (gen() % 3) {
        case 0: { // Reducible application
            auto node = std::make_shared<LambdaNode>();
            node->type = LambdaNode::APP;
            node->left = GenerateReducibleLambda(maxDepth - 1);
            node->right = GenerateReducibleLambda(maxDepth - 1);
            return node;
        }
        case 1: { // Abstraction
            auto node = std::make_shared<LambdaNode>();
            node->type = LambdaNode::ABS;
            node->var = vars[gen() % 4];
            node->left = GenerateReducibleLambda(maxDepth - 1);
            return node;
        }
        default: { // Variable
            auto node = std::make_shared<LambdaNode>();
            node->type = LambdaNode::VAR;
            node->var = vars[gen() % 4];
            return node;
        }
    }
}

std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return nullptr;
    
    switch (node->type) {
        case LambdaNode::APP:
            if (node->left && node->left->type == LambdaNode::ABS) {
                // Return the body of the abstraction (naive substitution)
                return node->left->left;
            }
            return std::make_shared<LambdaNode>(*node);
        case LambdaNode::ABS:
        case LambdaNode::VAR:
            return std::make_shared<LambdaNode>(*node);
    }
    return nullptr; // Added missing return
}

// Circuit rendering
void RenderLambdaCircuit(const std::shared_ptr<LambdaNode>& node, 
                        int x, int y, 
                        int width, int height) {
    const int PAD = 8;
    const int GATE_SIZE = 12;
    
    if (!node) return;

    switch (node->type) {
        case LambdaNode::ABS: {
            // Lambda gate (red rectangle)
            FillRect(x + width - GATE_SIZE - PAD, y + PAD, 
                    GATE_SIZE, GATE_SIZE, GATE_COLOR);
            
            // Input/output wires
            DrawLine(x + width/2, y + height/4, 
                    x + width/2, y + height/2, WIRE_COLOR);
            
            // Body circuit
            RenderLambdaCircuit(node->left, 
                              x + PAD, y + height/2, 
                              width - PAD*2, height/2);
            break;
        }
        case LambdaNode::APP: {
            // Junction (yellow circle)
            FillCircle(x + width/2, y + height/2, 4, PORT_COLOR);
            
            // Connection wires
            DrawLine(x + width/2, y + PAD, 
                    x + width/2, y + height/2, WIRE_COLOR);
            DrawLine(x + PAD, y + height/2, 
                    x + width - PAD, y + height/2, WIRE_COLOR);
            
            // Sub-circuits
            RenderLambdaCircuit(node->left, 
                              x + PAD, y + height/2 + PAD, 
                              width/2 - PAD*2, height/2 - PAD*2);
            RenderLambdaCircuit(node->right, 
                              x + width/2 + PAD, y + height/2 + PAD, 
                              width/2 - PAD*2, height/2 - PAD*2);
            break;
        }
        case LambdaNode::VAR: {
            // Terminal (blue diamond)
            const int d = GATE_SIZE/2;
            SDL_Point points[4] = {
                {x + width/2, y + height/2 - d},
                {x + width/2 + d, y + height/2},
                {x + width/2, y + height/2 + d},
                {x + width/2 - d, y + height/2}
            };
            FillPolygon(points, 4, TERMINAL_COLOR);
            break;
        }
    }
}