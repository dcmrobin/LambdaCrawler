#include "HelperFunctions.h"
#include "GFX.h"
#include "Map.h"

KeyStates keystates;

std::shared_ptr<LambdaNode> currentLambda;

void UpdateKeyStates() {
  // Save previous states
  keystates.upPressedPrev = keystates.upPressed;
  keystates.downPressedPrev = keystates.downPressed;
  keystates.zPressedPrev = keystates.zPressed;
  keystates.xPressedPrev = keystates.xPressed;
  keystates.leftPressedPrev = keystates.leftPressed;
  keystates.rightPressedPrev = keystates.rightPressed;
  keystates.startPressedPrev = keystates.startPressed;
  keystates.enterPressedPrev = keystates.enterPressed;

  // Read current states
  const Uint8* keystate = SDL_GetKeyboardState(NULL);
  keystates.upPressed = keystate[SDL_SCANCODE_UP];
  keystates.downPressed = keystate[SDL_SCANCODE_DOWN];
  keystates.leftPressed = keystate[SDL_SCANCODE_LEFT];
  keystates.rightPressed = keystate[SDL_SCANCODE_RIGHT];
  keystates.zPressed = keystate[SDL_SCANCODE_Z];
  keystates.xPressed = keystate[SDL_SCANCODE_X];
  keystates.startPressed = keystate[SDL_SCANCODE_ESCAPE];
  keystates.enterPressed = keystate[SDL_SCANCODE_RETURN];
}

bool IsKeyPressed(InputKey key, bool held) {
    if (!held)
    {
        switch (key) {
            case KEY_UP:    return keystates.upPressed && !keystates.upPressedPrev;
            case KEY_DOWN:  return keystates.downPressed && !keystates.downPressedPrev;
            case KEY_LEFT:  return keystates.leftPressed && !keystates.leftPressedPrev;
            case KEY_RIGHT: return keystates.rightPressed && !keystates.rightPressedPrev;
            case KEY_Z:     return keystates.zPressed && !keystates.zPressedPrev;
            case KEY_X:     return keystates.xPressed && !keystates.xPressedPrev;
            case KEY_START: return keystates.startPressed && !keystates.startPressedPrev;
            case KEY_ENTER: return keystates.enterPressed && !keystates.enterPressedPrev;
            default:        return false;
        }
    }
    else
    {
        switch (key) {
            case KEY_UP:    return keystates.upPressed;
            case KEY_DOWN:  return keystates.downPressed;
            case KEY_LEFT:  return keystates.leftPressed;
            case KEY_RIGHT: return keystates.rightPressed;
            case KEY_Z:     return keystates.zPressed;
            case KEY_X:     return keystates.xPressed;
            case KEY_START: return keystates.startPressed;
            case KEY_ENTER: return keystates.enterPressed;
            default:        return false;
        }
    }
}

bool Intersects(Hitbox& current, Hitbox other){
    return (current.x < other.x + other.width &&
            current.x + current.width > other.x &&
            current.y < other.y + other.height &&
            current.y + current.height > other.y);
}

void ChangeTile(int x, int y, bool solid, TileType type) {
    for (auto& tile : mapTiles) {
        if (tile.x == x && tile.y == y) {
            tile.solid = solid;
            tile.type = type;
            return;
        }
    }
    // If not found, create a new tile
    Tile newTile;
    newTile.x = x;
    newTile.y = y;
    newTile.hitbox = {x, y, tileSize, tileSize};
    newTile.solid = solid;
    newTile.type = type;
    mapTiles.push_back(newTile);
}