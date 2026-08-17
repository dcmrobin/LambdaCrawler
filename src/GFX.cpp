#include "GFX.h"
#include <sstream>

// Globals
int cameraX = 0;
int cameraY = 0;
extern SDL_Renderer* renderer;
extern SDL_Window* window;
std::unordered_map<std::string, SDL_Texture*> sprites;
std::unordered_map<std::string, TTF_Font*> fonts;

// Cursor
static std::string current_cursor_sprite;
SDL_Point cursor_hotspot = {0, 0};
static bool system_cursor_visible = false;

std::vector<std::string> sprite_names = {
    "cursor_normal",
    "player_idle",
    "player_idle_back",
    "player_idle_hop",
    "player_idle_back_hop",
    "player_idle_side",
    "player_idle_side_hop",
    "tile",
    "tile_ground",
    "tile_wall",
    "tile_wall_top",
    "tile_chute_closed",
    "tile_chute_open",
    "menu_splash",
    "title_sprite",
    "button",
    "button_highlighted",
    "button_pressed",
    "exitButton",
    "exitButton_highlighted",
    "exitButton_pressed"
};

void Init(SDL_Renderer* ren) {
    renderer = ren;
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
}

void SetCustomCursor(const std::string& sprite_name, int hot_x, int hot_y) {
    current_cursor_sprite = sprite_name;
    cursor_hotspot = {hot_x, hot_y};
}

void ShowSystemCursor(bool show) {
    system_cursor_visible = show;
    SDL_ShowCursor(show ? SDL_ENABLE : SDL_DISABLE);
}

void DrawCustomCursor() {
    if (system_cursor_visible || current_cursor_sprite.empty()) return;
    
    int mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);
    
    // Convert screen coordinates to logical coordinates
    float logical_x, logical_y;
    SDL_RenderWindowToLogical(renderer, mouse_x, mouse_y, &logical_x, &logical_y);
    
    // Draw cursor in logical space with hotspot offset
    DrawSprite(current_cursor_sprite, (int)(logical_x - cursor_hotspot.x), (int)(logical_y - cursor_hotspot.y));
}

void Cleanup() {
    for (auto& pair : sprites) {
        SDL_DestroyTexture(pair.second);
    }
    sprites.clear();
    IMG_Quit();

    for (auto& pair : fonts) {
        TTF_CloseFont(pair.second);
    }
    fonts.clear();
    TTF_Quit();
}

void LoadSprite(const std::string& name, const char* path) {
    SDL_Surface* surface = IMG_Load(path);
    if (!surface) {
        std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return;
    }

    sprites[name] = texture;
}

void DrawSprite(const std::string& name, int x, int y, int w, int h) {
    auto it = sprites.find(name);
    if (it == sprites.end()) {
        std::cerr << "Sprite not found: " << name << std::endl;
        return;
    }

    SDL_Rect dest = { x, y, w, h };
    SDL_RenderCopy(renderer, it->second, nullptr, &dest);
}

// Draw outline rectangle
void DrawRect(int x, int y, int w, int h, SDL_Color color) {
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

// Fill rectangle
void FillRect(int x, int y, int w, int h, SDL_Color color) {
    SDL_Rect rect = { x, y, w, h };
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// Draw line
void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

// Midpoint circle drawing (Bresenham-style)
void DrawCircle(int cx, int cy, int radius, SDL_Color color) {
    int x = radius;
    int y = 0;
    int err = 0;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, cx + x, cy + y);
        SDL_RenderDrawPoint(renderer, cx + y, cy + x);
        SDL_RenderDrawPoint(renderer, cx - x, cy + y);
        SDL_RenderDrawPoint(renderer, cx - y, cy + x);
        SDL_RenderDrawPoint(renderer, cx + x, cy - y);
        SDL_RenderDrawPoint(renderer, cx + y, cy - x);
        SDL_RenderDrawPoint(renderer, cx - x, cy - y);
        SDL_RenderDrawPoint(renderer, cx - y, cy - x);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}

// Fill circle using scanlines
void FillCircle(int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int dy = -radius; dy <= radius; dy++) {
        for (int dx = -radius; dx <= radius; dx++) {
            if (dx*dx + dy*dy <= radius*radius) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}

// Fill polygon using SDL_RenderGeometry
void FillPolygon(const SDL_Point* points, int count, SDL_Color color) {
    if (count < 3) return;

    std::vector<SDL_Vertex> vertices;
    for (int i = 0; i < count; i++) {
        vertices.push_back({
            {
                (float)(points[i].x),
                (float)(points[i].y)
            },
            {color.r, color.g, color.b, color.a},
            {0, 0}
        });
    }

    // For quadrilaterals (4 points), create two triangles
    std::vector<int> indices;
    if (count == 4) {
        // First triangle: points 0, 1, 2
        indices.push_back(0);
        indices.push_back(1);
        indices.push_back(2);
        // Second triangle: points 0, 2, 3
        indices.push_back(0);
        indices.push_back(2);
        indices.push_back(3);
    } else {
        // For other polygon types, use triangle fan
        for (int i = 0; i < count; i++) {
            indices.push_back(i);
        }
    }

    SDL_RenderGeometry(renderer, nullptr, 
                      vertices.data(), vertices.size(),
                      indices.data(), indices.size());
}

void LoadFont(const std::string& name, const char* path, int size) {
    TTF_Font* font = TTF_OpenFont(path, size);
    if (!font) {
        std::cerr << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        return;
    }
    fonts[name] = font;
}

void DrawText(const std::string& font_name, const std::string& text, int x, int y, SDL_Color color, float scale, TextAlignment alignment) {
    auto it = fonts.find(font_name);
    if (it == fonts.end()) return;

    SDL_Surface* surface = TTF_RenderText_Solid(it->second, text.c_str(), color);
    if (!surface) return;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) return;

    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

    // Apply scaling to the dimensions
    int scaledW = static_cast<int>(w * scale);
    int scaledH = static_cast<int>(h * scale);

    // Adjust X and Y based on alignment BEFORE camera offsets
    if (alignment == ALIGN_CENTER) {
        x = x - (scaledW / 2);
        y = y - (scaledH / 2);
    } else if (alignment == ALIGN_RIGHT) {
        x = x - scaledW;
    }

    // Apply camera scrolling
    x -= cameraX;
    y -= cameraY;

    SDL_Rect dest = { x, y, scaledW, scaledH };
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);
}

// function to draw a text box with text wrapped inside it:
void DrawTextBox(const std::string& font_name, const std::string& text, int x, int y, int box_width, int box_height, SDL_Color color, float scale, TextAlignment alignment) {
    auto it = fonts.find(font_name);
    if (it == fonts.end()) return;

    TTF_Font* font = it->second;
    int line_spacing = TTF_FontLineSkip(font);
    int max_line_width = static_cast<int>(box_width / scale);

    std::vector<std::string> lines;
    std::string current_line;
    std::istringstream words(text);
    std::string word;

    while (words >> word) {
        std::string test_line = current_line.empty() ? word : current_line + " " + word;
        int test_width;
        TTF_SizeText(font, test_line.c_str(), &test_width, nullptr);

        if (test_width > max_line_width) {
            if (!current_line.empty()) {
                lines.push_back(current_line);
            }
            current_line = word; // Start a new line with the current word
        } else {
            current_line = test_line; // Add the word to the current line
        }
    }

    if (!current_line.empty()) {
        lines.push_back(current_line); // Add the last line
    }

    // Get the boundaries of the text, and draw a rect around it
    int text_height = static_cast<int>(lines.size() * line_spacing * scale);
    DrawRect(x - (box_width / 2), y - (scale * 20), box_width, text_height + 4, {color.r, color.g, color.b, 255});

    // Draw each line of text
    for (size_t i = 0; i < lines.size(); ++i) {
        int line_y = y + static_cast<int>(i * line_spacing * scale);
        DrawText(font_name, lines[i], x, line_y, color, scale, alignment);
    }
}