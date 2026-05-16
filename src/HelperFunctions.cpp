#include "HelperFunctions.h"

KeyStates keystates;
GameStates gameState = STATE_MENU;
// Camera state
float cameraXF = 0.0f;
float cameraYF = 0.0f;
const float CAMERA_LERP = 0.15f;

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

void UpdateCamera() {
    // Camera follows player smoothly if currentLevel > 0
    if (currentLevel > 0) {
        int targetX = player.x + player.hitbox.width / 2 - LOGICAL_WIDTH / 2;
        int targetY = player.y + player.hitbox.height / 2 - LOGICAL_HEIGHT / 2;
        // Clamp target
        targetX = std::max(0, std::min(targetX, (mapWidth * tileSize) - LOGICAL_WIDTH));
        targetY = std::max(0, std::min(targetY, (mapHeight * tileSize) - LOGICAL_HEIGHT));
        // Smoothly interpolate
        cameraXF += (targetX - cameraXF) * CAMERA_LERP;
        cameraYF += (targetY - cameraYF) * CAMERA_LERP;
        cameraX = static_cast<int>(cameraXF);
        cameraY = static_cast<int>(cameraYF);
    } else {
        cameraXF = cameraX = 0;
        cameraYF = cameraY = 0;
    }
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