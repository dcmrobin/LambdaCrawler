#pragma once
#include <SDL.h>
#include <string>
#include <unordered_map>

#define NUM_SPRITES 1

extern SDL_Renderer* renderer;
extern std::unordered_map<std::string, SDL_Texture*> sprites;
extern SDL_Rect viewport;
extern const int LOGICAL_WIDTH;
extern const int LOGICAL_HEIGHT;

extern std::string sprite_names[NUM_SPRITES];

// Initialize with renderer
void Init(SDL_Renderer* renderer);

// Load a sprite (call this during setup)
void LoadSprite(const std::string& name, const char* path);

// Draw a sprite at logical coordinates (with automatic scaling)
void DrawSprite(const std::string& name, int x, int y, int w = 16, int h = 16);

// Cleanup (call before quitting)
void Cleanup();