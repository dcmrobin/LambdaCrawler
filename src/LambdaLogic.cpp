#include "LambdaLogic.h"
#include <random>
#include <queue>
#include <iostream>
#include <functional>

// --- Colors ---
const SDL_Color WIRE_COLOR      = {100, 255, 100, 255};   // Neon green
const SDL_Color GATE_COLOR      = {255, 50, 50, 255};     // Red (λ)
const SDL_Color TERMINAL_COLOR  = {100, 100, 255, 255};   // Blue (var)
const SDL_Color PORT_COLOR      = {255, 255, 100, 255};   // Yellow (application)
const SDL_Color GRID_COLOR      = {20, 40, 20, 255};      // Dark grid

static const char vars[] = {'a', 'b', 'c', 'd'};

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

// Always generates a reducible expression: an application where the left is an abstraction
std::shared_ptr<LambdaNode> GenerateReducibleLambda(int maxDepth) {
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto RandVar = [&]() {
        return vars[gen() % (sizeof(vars) / sizeof(vars[0]))];
    };

    // Declare a std::function so we can call RandomNode recursively
    std::function<std::shared_ptr<LambdaNode>(int)> RandomNode;

    RandomNode = [&](int depth) -> std::shared_ptr<LambdaNode> {
        if (depth <= 0) {
            return MakeVar(RandVar());
        }

        int choice = gen() % 3;
        if (choice == 0) { // VAR
            return MakeVar(RandVar());
        } else if (choice == 1) { // ABS
            return MakeAbs(RandVar(), RandomNode(depth - 1));
        } else { // APP
            return MakeApp(RandomNode(depth - 1), RandomNode(depth - 1));
        }
    };

    // Core reducible pattern: ( (\x. something) somethingElse )
    auto abstraction = MakeAbs(RandVar(), RandomNode(maxDepth - 1));
    auto argument = RandomNode(maxDepth - 1);

    std::shared_ptr<LambdaNode> root = MakeApp(abstraction, argument);

    int extraLayers = gen() % 3;
    for (int i = 0; i < extraLayers; ++i) {
        int wrap = gen() % 2;
        if (wrap == 0)
            root = MakeAbs(RandVar(), root); // wrap in abstraction
        else
            root = MakeApp(root, RandomNode(maxDepth - 1)); // add application
    }

    return root;
}


// --- Naive Beta Reduction ---
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

// --- Grid Logic ---
struct GridCell {
    int gridX, gridY;
    bool occupied = false;
    std::shared_ptr<LambdaNode> node = nullptr;
};

using Grid = std::vector<std::vector<GridCell>>;

bool IsValid(int x, int y, int cols, int rows) {
    return x >= 0 && y >= 0 && x < cols && y < rows;
}

std::optional<std::pair<int, int>> FindFreeNeighbor(Grid& grid, int x, int y, int cols, int rows) {
    std::queue<std::pair<int, int>> q;
    q.push({x, y});

    const int dx[4] = {-1, 1, 0, 0};
    const int dy[4] = {0, 0, -1, 1};

    while (!q.empty()) {
        auto [cx, cy] = q.front();
        q.pop();

        for (int d = 0; d < 4; ++d) {
            int nx = cx + dx[d], ny = cy + dy[d];
            if (IsValid(nx, ny, cols, rows) && !grid[ny][nx].occupied) {
                return {{nx, ny}};
            }
        }
    }
    return std::nullopt;
}

void RenderLambdaGrid(const std::shared_ptr<LambdaNode>& root, int startX, int startY, int cols, int rows, int cellW, int cellH) {
    Grid grid(rows, std::vector<GridCell>(cols));
    for (int y = 0; y < rows; ++y)
        for (int x = 0; x < cols; ++x)
            grid[y][x] = {x, y};

    std::queue<std::tuple<std::shared_ptr<LambdaNode>, int, int>> q;
    int cx = cols / 2, cy = 0;

    grid[cy][cx].occupied = true;
    grid[cy][cx].node = root;
    q.push({root, cx, cy});

    while (!q.empty()) {
        auto [node, gx, gy] = q.front();
        q.pop();

        int px = startX + gx * cellW;
        int py = startY + gy * cellH;

        // Draw grid cell background
        DrawRect(px, py, cellW, cellH, GRID_COLOR);

        // Render node
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

        // Children
        auto connect = [&](std::shared_ptr<LambdaNode> child) {
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
            DrawLine(x1, y1, x2, y2, WIRE_COLOR);
        };

        connect(node->left);
        connect(node->right);
    }
}

void DrawConnection(int x1, int y1, int x2, int y2, SDL_Color color) {
    DrawLine(x1, y1, x2, y2, color);
}

std::string LambdaToString(const std::shared_ptr<LambdaNode>& node) {
    if (!node) return "";

    switch (node->type) {
        case LambdaNode::VAR:
            return std::string(1, node->var);
        case LambdaNode::ABS:
            return "(\\" + std::string(1, node->var) + "." + LambdaToString(node->left) + ")";
        case LambdaNode::APP:
            return "(" + LambdaToString(node->left) + " " + LambdaToString(node->right) + ")";
    }
    return "";
}