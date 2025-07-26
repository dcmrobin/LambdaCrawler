#include "GFX.h"
#include <SDL_image.h>
#include <iostream>
#include <cmath>

// Globals
SDL_Renderer* renderer = nullptr;
std::unordered_map<std::string, SDL_Texture*> sprites;

// Logical resolution defined elsewhere
extern const int LOGICAL_WIDTH;
extern const int LOGICAL_HEIGHT;
extern SDL_Rect viewport;

std::vector<std::string> sprite_names = {
    "player_idle",
};

void Init(SDL_Renderer* ren) {
    renderer = ren;
    IMG_Init(IMG_INIT_PNG);
}

void Cleanup() {
    for (auto& pair : sprites) {
        SDL_DestroyTexture(pair.second);
    }
    sprites.clear();
    IMG_Quit();
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

    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    SDL_Rect dest = {
        viewport.x + (int)(x * scale_x),
        viewport.y + (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };

    SDL_RenderCopy(renderer, it->second, nullptr, &dest);
}

// Draw outline rectangle
void DrawRect(int x, int y, int w, int h, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect rect = {
        viewport.x + (int)(x * scale_x),
        viewport.y + (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };

    SDL_RenderDrawRect(renderer, &rect);
}

// Fill rectangle
void FillRect(int x, int y, int w, int h, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect rect = {
        viewport.x + (int)(x * scale_x),
        viewport.y + (int)(y * scale_y),
        (int)(w * scale_x),
        (int)(h * scale_y)
    };

    SDL_RenderFillRect(renderer, &rect);
}

// Draw line
void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_RenderDrawLine(
        renderer,
        viewport.x + (int)(x1 * scale_x),
        viewport.y + (int)(y1 * scale_y),
        viewport.x + (int)(x2 * scale_x),
        viewport.y + (int)(y2 * scale_y)
    );
}

// Midpoint circle drawing (Bresenham-style)
void DrawCircle(int cx, int cy, int radius, SDL_Color color) {
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    int x = radius;
    int y = 0;
    int err = 0;

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    while (x >= y) {
        int px = (int)(cx * scale_x);
        int py = (int)(cy * scale_y);
        int sx = (int)(x * scale_x);
        int sy = (int)(y * scale_y);

        SDL_RenderDrawPoint(renderer, viewport.x + px + sx, viewport.y + py + sy);
        SDL_RenderDrawPoint(renderer, viewport.x + px + sy, viewport.y + py + sx);
        SDL_RenderDrawPoint(renderer, viewport.x + px - sx, viewport.y + py + sy);
        SDL_RenderDrawPoint(renderer, viewport.x + px - sy, viewport.y + py + sx);
        SDL_RenderDrawPoint(renderer, viewport.x + px + sx, viewport.y + py - sy);
        SDL_RenderDrawPoint(renderer, viewport.x + px + sy, viewport.y + py - sx);
        SDL_RenderDrawPoint(renderer, viewport.x + px - sx, viewport.y + py - sy);
        SDL_RenderDrawPoint(renderer, viewport.x + px - sy, viewport.y + py - sx);

        y += 1;
        err += 1 + 2*y;
        if (2*(err - x) + 1 > 0) {
            x -= 1;
            err += 1 - 2*x;
        }
    }
}
