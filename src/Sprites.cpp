#include "Sprites.h"
#include <SDL_image.h>
#include <iostream>

// Static variables
SDL_Renderer* renderer = nullptr;
std::unordered_map<std::string, SDL_Texture*> sprites;

std::string sprite_names[NUM_SPRITES] = {
    "player_idle",
};

void Init(SDL_Renderer* renderer) {
    renderer = renderer;
    IMG_Init(IMG_INIT_PNG);  // Initialize SDL_image
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

    sprites[name] = texture;  // Store the texture
}

void DrawSprite(const std::string& name, int x, int y, int w, int h) {
    auto it = sprites.find(name);
    if (it == sprites.end()) {
        std::cerr << "Sprite not found: " << name << std::endl;
        return;
    }

    // Convert logical to physical coordinates
    float scale_x = (float)viewport.w / LOGICAL_WIDTH;
    float scale_y = (float)viewport.h / LOGICAL_HEIGHT;

    SDL_Rect dest = {
        viewport.x + (int)(x * scale_x),  // X position (with viewport offset)
        viewport.y + (int)(y * scale_y),  // Y position
        (int)(w * scale_x),               // Scaled width
        (int)(h * scale_y)                // Scaled height
    };

    SDL_RenderCopy(renderer, it->second, nullptr, &dest);
}

void Cleanup() {
    for (auto& pair : sprites) {
        SDL_DestroyTexture(pair.second);
    }
    sprites.clear();
    IMG_Quit();
}