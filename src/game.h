#pragma once
#include "HelperFunctions.h"

extern int cameraX;
extern int cameraY;
extern bool showSettings;
extern bool aSettingHasChanged;
extern int originalVolumeSliderValue;
extern int mainVolume;
extern bool settingsApplied;

void game_setup();
void game_loop();
void UpdateGame();
void RenderGame();
void UpdateMenu();