#include "LambdaLogic.h"
#include <random>
#include <queue>
#include <iostream>
#include <functional>
#include <algorithm>
#include <cmath>

// --- Colors ---
const SDL_Color WIRE_COLOR      = {100, 255, 100, 255};   // Default: Neon green
const SDL_Color GATE_COLOR      = {255, 50, 50, 255};     // Red (λ)
const SDL_Color TERMINAL_COLOR  = {100, 100, 255, 255};   // Blue (var)
const SDL_Color PORT_COLOR      = {255, 255, 100, 255};   // Yellow (application)
const SDL_Color GRID_COLOR      = {20, 40, 20, 255};      // Dark grid

static const char vars[] = {'a', 'b', 'c', 'd'};

// Node helpers
std::shared_ptr<LambdaNode> MakeVar(char v) {
    auto node = std::make_shared<LambdaNode>();
    node->type = LambdaNode::VAR;
    node->var = v;
    return node;
}

std::shared_ptr<LambdaNode> MakeAbs(char var, std::shared_ptr<LambdaNode> body) {
    auto node = std::make_shared<LambdaNode>();
    node->type = LambdaNode::ABS;
    node->var = var;
    node->left = body;
    return node;
}

std::shared_ptr<LambdaNode> MakeApp(std::shared_ptr<LambdaNode> left, std::shared_ptr<LambdaNode> right) {
    auto node = std::make_shared<LambdaNode>();
    node->type = LambdaNode::APP;
    node->left = left;
    node->right = right;
    return node;
}

std::set<char> GetFreeVariables(const std::shared_ptr<LambdaNode>& node) {
    std::set<char> freeVars;
    if (!node) return freeVars;
    
    switch (node->type) {
        case LambdaNode::VAR:
            freeVars.insert(node->var);
            break;
        case LambdaNode::ABS: {
            auto bodyFree = GetFreeVariables(node->left);
            bodyFree.erase(node->var); // Remove the bound variable
            freeVars.insert(bodyFree.begin(), bodyFree.end());
            break;
        }
        case LambdaNode::APP: {
            auto leftFree = GetFreeVariables(node->left);
            auto rightFree = GetFreeVariables(node->right);
            freeVars.insert(leftFree.begin(), leftFree.end());
            freeVars.insert(rightFree.begin(), rightFree.end());
            break;
        }
    }
    return freeVars;
}

std::set<char> GetBoundVariables(const std::shared_ptr<LambdaNode>& node) {
    std::set<char> boundVars;
    if (!node) return boundVars;
    
    switch (node->type) {
        case LambdaNode::VAR:
            break;
        case LambdaNode::ABS:
            boundVars.insert(node->var);
            {
                auto bodyBound = GetBoundVariables(node->left);
                boundVars.insert(bodyBound.begin(), bodyBound.end());
            }
            break;
        case LambdaNode::APP: {
            auto leftBound = GetBoundVariables(node->left);
            auto rightBound = GetBoundVariables(node->right);
            boundVars.insert(leftBound.begin(), leftBound.end());
            boundVars.insert(rightBound.begin(), rightBound.end());
            break;
        }
    }
    return boundVars;
}

char GenerateFreshVariable(const std::set<char>& usedVariables) {
    // Try a-z
    for (char c = 'a'; c <= 'z'; c++) {
        if (usedVariables.find(c) == usedVariables.end()) {
            return c;
        }
    }
    
    // If all a-z are used, try A-Z
    for (char c = 'A'; c <= 'Z'; c++) {
        if (usedVariables.find(c) == usedVariables.end()) {
            return c;
        }
    }
    
    // As a fallback, return 'x' (this shouldn't happen in normal cases)
    return 'x';
}

std::shared_ptr<LambdaNode> AlphaRename(const std::shared_ptr<LambdaNode>& node, char oldVar, char newVar) {
    if (!node) return nullptr;
    
    switch (node->type) {
        case LambdaNode::VAR:
            if (node->var == oldVar) {
                return MakeVar(newVar);
            }
            return node;
        case LambdaNode::ABS:
            if (node->var == oldVar) {
                // This abstraction binds the variable we're renaming, so we stop here
                return node;
            }
            return MakeAbs(node->var, AlphaRename(node->left, oldVar, newVar));
        case LambdaNode::APP:
            return MakeApp(
                AlphaRename(node->left, oldVar, newVar),
                AlphaRename(node->right, oldVar, newVar)
            );
    }
    return node;
}

std::shared_ptr<LambdaNode> Substitute(const std::shared_ptr<LambdaNode>& node, char var, const std::shared_ptr<LambdaNode>& replacement) {
    if (!node) return nullptr;
    
    switch (node->type) {
        case LambdaNode::VAR:
            if (node->var == var) {
                return replacement;
            }
            return node;
        case LambdaNode::ABS:
            if (node->var == var) {
                // The variable is bound in this abstraction, so we don't substitute
                return node;
            }
            
            {
                // Check if the replacement has free variables that would be captured
                auto freeInReplacement = GetFreeVariables(replacement);
                if (freeInReplacement.find(node->var) != freeInReplacement.end()) {
                    // We need to alpha-rename the bound variable to avoid capture
                    char freshVar = GenerateFreshVariable(GetFreeVariables(node));
                    auto renamedBody = AlphaRename(node->left, node->var, freshVar);
                    return MakeAbs(freshVar, Substitute(renamedBody, var, replacement));
                }
            }
            
            // Safe to substitute in the body
            return MakeAbs(node->var, Substitute(node->left, var, replacement));
        case LambdaNode::APP:
            return MakeApp(
                Substitute(node->left, var, replacement),
                Substitute(node->right, var, replacement)
            );
    }
    return node;
}

// Replace the existing BetaReduce function with this complete implementation
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node, ReductionStrategy strategy) {
    if (!node) return nullptr;
    
    switch (strategy) {
        case ReductionStrategy::NORMAL_ORDER:
            return NormalOrderReduce(node);
        case ReductionStrategy::APPLICATIVE_ORDER:
            return ApplicativeOrderReduce(node);
        case ReductionStrategy::CALL_BY_NAME:
            return CallByNameReduce(node);
        case ReductionStrategy::CALL_BY_NEED:
            // For simplicity, we'll use call-by-name for now
            // True call-by-need requires memoization which is more complex
            return CallByNameReduce(node);
    }
    return node;
}

// Normal order: leftmost, outermost redex first
std::shared_ptr<LambdaNode> NormalOrderReduce(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return nullptr;
    
    // If this is a redex, reduce it (outermost)
    if (node->type == LambdaNode::APP && 
        node->left && node->left->type == LambdaNode::ABS) {
        return Substitute(node->left->left, node->left->var, node->right);
    }
    
    // Otherwise, traverse left to right
    switch (node->type) {
        case LambdaNode::VAR:
            return node;
        case LambdaNode::ABS:
            return MakeAbs(node->var, NormalOrderReduce(node->left));
        case LambdaNode::APP: {
            // Try reducing left subtree first
            auto reducedLeft = NormalOrderReduce(node->left);
            if (reducedLeft != node->left) {
                return MakeApp(reducedLeft, node->right);
            }
            // If left didn't change, try right subtree
            auto reducedRight = NormalOrderReduce(node->right);
            if (reducedRight != node->right) {
                return MakeApp(node->left, reducedRight);
            }
            return node;
        }
    }
    return node;
}

// Applicative order: leftmost, innermost redex first  
std::shared_ptr<LambdaNode> ApplicativeOrderReduce(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return nullptr;
    
    switch (node->type) {
        case LambdaNode::VAR:
            return node;
        case LambdaNode::ABS:
            return MakeAbs(node->var, ApplicativeOrderReduce(node->left));
        case LambdaNode::APP: {
            // First reduce both sides completely (innermost)
            auto reducedLeft = ApplicativeOrderReduce(node->left);
            auto reducedRight = ApplicativeOrderReduce(node->right);
            
            // If we have (λx.M) N after reduction, do beta reduction
            if (reducedLeft->type == LambdaNode::ABS) {
                return Substitute(reducedLeft->left, reducedLeft->var, reducedRight);
            }
            
            if (reducedLeft != node->left || reducedRight != node->right) {
                return MakeApp(reducedLeft, reducedRight);
            }
            return node;
        }
    }
    return node;
}

// Call-by-name: like normal order but don't reduce inside abstractions
std::shared_ptr<LambdaNode> CallByNameReduce(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return nullptr;
    
    // Only reduce at application nodes, not inside abstractions
    if (node->type == LambdaNode::APP && 
        node->left && node->left->type == LambdaNode::ABS) {
        return Substitute(node->left->left, node->left->var, node->right);
    }
    
    // For applications, only reduce the left side (the function)
    if (node->type == LambdaNode::APP) {
        auto reducedLeft = CallByNameReduce(node->left);
        if (reducedLeft != node->left) {
            return MakeApp(reducedLeft, node->right);
        }
    }
    
    return node;
}

// Helper to check if a node contains a redex
bool HasRedex(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return false;
    
    if (node->type == LambdaNode::APP && 
        node->left && node->left->type == LambdaNode::ABS) {
        return true;
    }
    
    switch (node->type) {
        case LambdaNode::VAR:
            return false;
        case LambdaNode::ABS:
            return HasRedex(node->left);
        case LambdaNode::APP:
            return HasRedex(node->left) || HasRedex(node->right);
    }
    return false;
}

// Enhanced Normalize function with strategy
std::shared_ptr<LambdaNode> Normalize(const std::shared_ptr<LambdaNode>& node, 
                                     ReductionStrategy strategy = ReductionStrategy::NORMAL_ORDER,
                                     int maxSteps = 1000) {
    auto current = node;
    for (int i = 0; i < maxSteps; i++) {
        auto next = BetaReduce(current, strategy);
        if (next == current) {
            return next; // No more reductions possible
        }
        current = next;
        
        // Optional: add debug output to see reduction steps
        // std::cout << "Step " << i << ": " << LambdaToString(current) << std::endl;
    }
    return current;
}

// Lambda string output
std::string LambdaToString(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return "";
    switch (node->type) {
        case LambdaNode::VAR: return std::string(1, node->var);
        case LambdaNode::ABS: return "(\\" + std::string(1, node->var) + "." + LambdaToString(node->left) + ")";
        case LambdaNode::APP: return "(" + LambdaToString(node->left) + " " + LambdaToString(node->right) + ")";
    }
    return "";
}

// Reducible generator
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto RandVar = [&]() {
        return vars[gen() % (sizeof(vars) / sizeof(vars[0]))];
    };

    std::function<std::shared_ptr<LambdaNode>(int)> RandomNode;

    RandomNode = [&](int depth) -> std::shared_ptr<LambdaNode> {
        if (depth <= 0) return MakeVar(RandVar());

        int choice = gen() % 3;
        if (choice == 0) return MakeVar(RandVar());
        if (choice == 1) return MakeAbs(RandVar(), RandomNode(depth - 1));
        return MakeApp(RandomNode(depth - 1), RandomNode(depth - 1));
    };

    auto abstraction = MakeAbs(RandVar(), RandomNode(maxDepth - 1));
    auto argument = RandomNode(maxDepth - 1);
    std::shared_ptr<LambdaNode> root = MakeApp(abstraction, argument);

    int extraLayers = gen() % 3;
    for (int i = 0; i < extraLayers; ++i) {
        if (gen() % 2)
            root = MakeAbs(RandVar(), root);
        else
            root = MakeApp(root, RandomNode(maxDepth - 1));
    }

    //std::cout << "Generated: " << LambdaToString(root) << std::endl;
    return root;
}

bool IsValid(int x, int y, int cols, int rows) {
    return x >= 0 && y >= 0 && x < cols && y < rows;
}

std::optional<std::pair<int, int>> FindFreeNeighbor(Grid& grid, int x, int y, int cols, int rows) {
    std::vector<std::pair<int, int>> directions = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0},
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1}
    };

    auto centerX = cols / 2;
    auto centerY = rows / 2;

    std::vector<std::pair<int, int>> candidates;
    for (auto [dx, dy] : directions) {
        int nx = x + dx, ny = y + dy;
        if (IsValid(nx, ny, cols, rows) && !grid[ny][nx].occupied) {
            candidates.emplace_back(nx, ny);
        }
    }

    if (candidates.empty()) return std::nullopt;

    std::sort(candidates.begin(), candidates.end(), [&](auto a, auto b) {
        int da = std::abs(a.first - centerX) + std::abs(a.second - centerY);
        int db = std::abs(b.first - centerX) + std::abs(b.second - centerY);
        return da < db;
    });

    return candidates.front();
}

// ORTHOGONAL DRAW
void DrawOrthogonalConnection(int x1, int y1, int x2, int y2, SDL_Color color,  const std::shared_ptr<LambdaNode>& node1, const std::shared_ptr<LambdaNode>& node2) {
    // Create a hash based on the nodes' structure
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
    
    // Use the hash to determine direction (even/odd)
    bool horizontalFirst = (hash % 2 == 0);

    if (horizontalFirst) {
        DrawLine(x1, y1, x2, y1, color);
        DrawLine(x2, y1, x2, y2, color);
    } else {
        DrawLine(x1, y1, x1, y2, color);
        DrawLine(x1, y2, x2, y2, color);
    }
}

// RENDER GRID-BASED
void RenderLambdaGrid(const std::shared_ptr<LambdaNode>& root, int startX, int startY, int cols, int rows, int cellW, int cellH) {
    Grid grid(rows, std::vector<GridCell>(cols));
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            grid[y][x] = {x, y};

    std::queue<std::tuple<std::shared_ptr<LambdaNode>, int, int>> q;
    int cx = cols / 2, cy = rows / 2;

    grid[cy][cx].occupied = true;
    grid[cy][cx].node = root;
    q.push({root, cx, cy});

    while (!q.empty()) {
        auto [node, gx, gy] = q.front();
        q.pop();

        int px = startX + gx * cellW;
        int py = startY + gy * cellH;

        DrawRect(px, py, cellW, cellH, GRID_COLOR);

        switch (node->type) {
            case LambdaNode::ABS:
                FillRect(px + 4, py + 4, cellW - 8, cellH - 8, GATE_COLOR);
                break;
            case LambdaNode::APP:
                FillCircle(px + cellW / 2, py + cellH / 2, cellW / 4, PORT_COLOR);
                break;
            case LambdaNode::VAR: {
                SDL_Point pts[4] = {// increase the 4s, smaller diamond. decrease the 4s, bigger diamond.
                    {px + cellW/2, py + 4},
                    {px + cellW - 4, py + cellH/2},
                    {px + cellW/2, py + cellH - 4},
                    {px + 4, py + cellH/2}
                };
                FillPolygon(pts, 4, TERMINAL_COLOR);
                break;
            }
        }

        auto connect = [&](std::shared_ptr<LambdaNode> child, LambdaNode::Type type) {
            if (!child) return;
            auto pos = FindFreeNeighbor(grid, gx, gy, cols, rows);
            if (!pos) return;

            int nx = pos->first, ny = pos->second;
            grid[ny][nx].occupied = true;
            grid[ny][nx].node = child;
            q.push({child, nx, ny});

            int x1 = startX + gx * cellW + cellW / 2;
            int y1 = startY + gy * cellH + cellH / 2;
            int x2 = startX + nx * cellW + cellW / 2;
            int y2 = startY + ny * cellH + cellH / 2;

            SDL_Color wireColor = WIRE_COLOR;
            if (type == LambdaNode::APP) wireColor = PORT_COLOR;
            if (type == LambdaNode::ABS) wireColor = GATE_COLOR;
            if (type == LambdaNode::VAR) wireColor = TERMINAL_COLOR;

            DrawOrthogonalConnection(x1, y1, x2, y2, wireColor, grid[gy][gx].node, grid[ny][nx].node);
        };

        connect(node->left, node->type);
        connect(node->right, node->type);
    }
}
