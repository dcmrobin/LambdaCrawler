#include "GFX.h"
#include <SDL_image.h>
#include <iostream>
#include <cmath>

// Globals
extern SDL_Renderer* renderer;
std::unordered_map<std::string, SDL_Texture*> sprites;
std::unordered_map<std::string, TTF_Font*> fonts;

// Logical resolution defined elsewhere
extern int LOGICAL_WIDTH;
extern int LOGICAL_HEIGHT;
extern SDL_Rect viewport;

std::vector<std::string> sprite_names = {
    "player_idle",
    "player_idle_back",
    "player_idle_hop",
    "player_idle_back_hop",
    "tile",
    "tile_ground",
    "tile_wall",
    "tile_wall_top",
    "tile_chute_closed",
    "tile_chute_open",
};

void Init(SDL_Renderer* ren) {
    renderer = ren;
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
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

    // Calculate scale factors
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    SDL_Rect dest = {
        (int)(x * scale_x),
        (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };
    
    // Set the viewport for this drawing operation
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_RenderCopy(renderer, it->second, nullptr, &dest);
}

// Draw outline rectangle
void DrawRect(int x, int y, int w, int h, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    SDL_Rect rect = {
        (int)(x * scale_x),
        (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };
    
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawRect(renderer, &rect);
}

// Fill rectangle
void FillRect(int x, int y, int w, int h, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    SDL_Rect rect = {
        (int)(x * scale_x),
        (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };
    
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(renderer, &rect);
}

// Draw line
void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(
        renderer,
        (int)(x1 * scale_x),
        (int)(y1 * scale_y),
        (int)(x2 * scale_x),
        (int)(y2 * scale_y)
    );
}

// Midpoint circle drawing (Bresenham-style)
void DrawCircle(int cx, int cy, int radius, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    int x = radius;
    int y = 0;
    int err = 0;

    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    while (x >= y) {
        SDL_RenderDrawPoint(renderer, (int)((cx + x) * scale_x), (int)((cy + y) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx + y) * scale_x), (int)((cy + x) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx - x) * scale_x), (int)((cy + y) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx - y) * scale_x), (int)((cy + x) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx + x) * scale_x), (int)((cy - y) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx + y) * scale_x), (int)((cy - x) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx - x) * scale_x), (int)((cy - y) * scale_y));
        SDL_RenderDrawPoint(renderer, (int)((cx - y) * scale_x), (int)((cy - x) * scale_y));

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
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;
    
    int scaled_cx = (int)(cx * scale_x);
    int scaled_cy = (int)(cy * scale_y);
    int scaled_radius = (int)(radius * std::max(scale_x, scale_y));

    SDL_RenderSetViewport(renderer, &viewport);
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    
    for (int dy = -scaled_radius; dy <= scaled_radius; dy++) {
        for (int dx = -scaled_radius; dx <= scaled_radius; dx++) {
            if (dx*dx + dy*dy <= scaled_radius*scaled_radius) {
                SDL_RenderDrawPoint(renderer, scaled_cx + dx, scaled_cy + dy);
            }
        }
    }
}

// Fill polygon using SDL_RenderGeometry
void FillPolygon(const SDL_Point* points, int count, SDL_Color color) {
    if (count < 3) return;

    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    std::vector<SDL_Vertex> vertices;
    for (int i = 0; i < count; i++) {
        vertices.push_back({
            {
                (float)(points[i].x * scale_x),
                (float)(points[i].y * scale_y)
            },
            {color.r, color.g, color.b, color.a},
            {0, 0}
        });
    }

    std::vector<int> indices;
    for (int i = 0; i < count; i++) {
        indices.push_back(i);
    }

    SDL_RenderSetViewport(renderer, &viewport);
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

void DrawText(const std::string& font_name, const std::string& text, 
             int x, int y, SDL_Color color, float scale) {
    auto it = fonts.find(font_name);
    if (it == fonts.end()) {
        std::cerr << "Font not found: " << font_name << std::endl;
        return;
    }

    // Render text to surface
    SDL_Surface* surface = TTF_RenderText_Solid(it->second, text.c_str(), color);
    if (!surface) {
        std::cerr << "TTF_RenderText Error: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
        return;
    }

    // Get original text dimensions
    int w, h;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);

    // Apply THREE scaling factors:
    // 1. Logical viewport scaling (existing)
    // 2. Custom scale parameter (new)
    // 3. Integer scaling for crisp text (recommended)
    float scale_x = (float)viewport.w / LOGICAL_WIDTH * scale;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT * scale;

    // For crisp text, use integer scaling if scale is close to 1.0
    if (scale >= 0.9f && scale <= 1.1f) {
        scale_x = 1.0f;
        scale_y = 1.0f;
    }

    SDL_Rect dest = {
        (int)(x * (float)viewport.w / LOGICAL_WIDTH),  // Viewport-scaled position
        (int)(y * (float)viewport.h / LOGICAL_HEIGHT),
        (int)(w * scale_x),  // Custom-scaled dimensions
        (int)(h * scale_y)
    };

    // Render the text
    SDL_RenderSetViewport(renderer, &viewport);
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
    SDL_DestroyTexture(texture);
}