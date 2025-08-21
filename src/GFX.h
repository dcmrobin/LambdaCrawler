#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <vector>

extern int cameraX;
extern int cameraY;
extern SDL_Renderer* renderer;
extern SDL_Rect viewport;
extern int LOGICAL_WIDTH;
extern int LOGICAL_HEIGHT;

// Sprite map
extern std::unordered_map<std::string, SDL_Texture*> sprites;

// Font map
extern std::unordered_map<std::string, TTF_Font*> fonts;

// Sprite names
extern std::vector<std::string> sprite_names;

// Init & cleanup
void Init(SDL_Renderer* renderer);
void Cleanup();

// Sprite loading & drawing
void LoadSprite(const std::string& name, const char* path);
void DrawSprite(const std::string& name, int x, int y, int w = 16, int h = 16);

// Mouse pointer
void SetCustomCursor(const std::string& sprite_name, int hot_x = 0, int hot_y = 0);
void ShowSystemCursor(bool show);
void DrawCustomCursor();

// Font loading & text drawing
void LoadFont(const std::string& name, const char* path, int size);
void DrawText(const std::string& font_name, const std::string& text, int x, int y, SDL_Color color, float scale);

// Shapes
void DrawRect(int x, int y, int w, int h, SDL_Color color);
void FillRect(int x, int y, int w, int h, SDL_Color color);
void DrawLine(int x1, int y1, int x2, int y2, SDL_Color color);
void DrawCircle(int cx, int cy, int radius, SDL_Color color);
void FillCircle(int cx, int cy, int radius, SDL_Color color);
void FillPolygon(const SDL_Point* points, int count, SDL_Color color);