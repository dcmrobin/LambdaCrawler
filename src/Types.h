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
  bool enterPressed;
  bool enterPressedPrev;
};

extern KeyStates keystates;

enum InputKey {
    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,
    KEY_Z,
    KEY_X,
    KEY_START,
    KEY_ENTER
};

enum GameStates {
    STATE_MENU,
    STATE_RUN,
    STATE_PAUSE
}; // Defining all possible game states

extern GameStates gameState;

struct Hitbox {
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
};

enum ButtonAction {
    BTN_NOTHING,
    BTN_START_GAME,
    BTN_LOAD_GAME,
    BTN_QUIT_APPLICATION,
    BTN_SHOW_SETTINGS,
    BTN_HIDE_SETTINGS,
    BTN_FULLSCREEN,
    BTN_WINDOWED,
    BTN_LOAD_SETTINGS,
    BTN_SAVE_SETTINGS,
    BTN_APPLY_SETTINGS
}; // Defining all possible button actions

struct Button {
    // Button position
    int x = 0;
    int y = 0;

    // Button width and height
    int width = 100;
    int height = 30;

    // Button text, text size, and text colour
    SDL_Color textColor = {63, 63, 63};
    float textSize = 0.368f;
    std::string text = "null";

    // Set the sprite the button starts as. As a default, the normal button sprite is assigned
    std::string spriteName = "button";

    // Original sprite name that can be defaulted back to when the spriteName gets changed
    std::string originalSpriteName = "button";

    // Set the default action that happens when the button is pressed to BTN_NOTHING
    ButtonAction action = BTN_NOTHING;

    // Several booleans to track what state the button is in
    bool highlighted = false;
    bool pressed = false;
    bool active = true;
};

extern std::vector<Button> buttons;

struct Slider {
    // Slider position
    int x = 0;
    int y = 0;

    // Slider width and height
    int width = 100;
    int height = 20;

    // Slider text and text colour
    SDL_Color textColor = {63, 63, 63};
    std::string text = "null";

    // Slider-specific properties
    float minValue = 0;
    float maxValue = 100;
    float value = 45;

    // Handle-specific properties
    bool handleHighlighted = false;
    bool handleGrabbed = false;
    bool handleActive = true;
};

extern std::vector<Slider> sliders;

enum TextAlignment {
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
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