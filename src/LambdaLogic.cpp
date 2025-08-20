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

// Trivial beta reducer
std::shared_ptr<LambdaNode> BetaReduce(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return nullptr;
    switch (node->type) {
        case LambdaNode::APP:
            if (node->left && node->left->type == LambdaNode::ABS)
                return node->left->left;
            return std::make_shared<LambdaNode>(*node);
        case LambdaNode::ABS:
        case LambdaNode::VAR:
            return std::make_shared<LambdaNode>(*node);
    }
    return nullptr;
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
                SDL_Point pts[4] = {
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
