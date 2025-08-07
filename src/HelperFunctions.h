#pragma once

struct KeyStates {
  bool upPressed;
  bool upPressedPrev;
  bool downPressed;
  bool downPressedPrev;
  bool zPressed;
  bool zPressedPrev;
  bool xPressed;
  bool xPressedPrev;
  bool leftPressed;
  bool leftPressedPrev;
  bool rightPressed;
  bool rightPressedPrev;
  bool startPressed;
  bool startPressedPrev;
};

extern KeyStates keystates;

enum InputKey {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_Z,
    KEY_X,
    KEY_START
};

struct Hitbox {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

enum TileType {
    ERR,
    GROUND,
    WALL,
    WALL_TOP,
    CHUTE_CLOSED,
    CHUTE_OPEN
};

struct Tile {
    TileType type;
    Hitbox hitbox;
    bool solid;
    int width;
    int height;
    int x;
    int y;
};

bool Intersects(Hitbox& current, Hitbox other);
void UpdateKeyStates();
bool IsKeyPressed(InputKey key, bool held);
void ChangeTile(int x, int y, bool solid, TileType type);