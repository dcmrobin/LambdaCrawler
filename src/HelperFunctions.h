#pragma once
#include <SDL.h>
#include <algorithm>
#include <SDL_ttf.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <SDL_image.h>
#include <cmath>
#include <iostream>
#include "Types.h"
#include "game.h"
#include "GFX.h"
#include "Player.h"
#include "Map.h"
#include "DisplayLogic.h"

extern float cameraXF;
extern float cameraYF;
extern const float CAMERA_LERP;

bool Intersects(Hitbox& current, Hitbox other);
void UpdateKeyStates();
void UpdateCamera();
bool IsKeyPressed(InputKey key, bool held);
void ChangeTile(int x, int y, bool solid, TileType type);