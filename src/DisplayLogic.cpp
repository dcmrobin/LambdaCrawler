#include "DisplayLogic.h"

int hopTimer = 0;
void RenderPlayer() {
    if (player.dy == -1 && player.spriteName != "player_idle_back_hop") {
        player.spriteName = "player_idle_back";
    } else if (player.dy == 1 && player.spriteName != "player_idle_hop") {
        player.spriteName = "player_idle";
    }

    if (player.moving) {
        hopTimer++;
        if (hopTimer == 2) {
            player.spriteName = player.spriteName == "player_idle_back" ? "player_idle_back_hop" : player.spriteName == "player_idle" ? "player_idle_hop" : player.spriteName;
        }
        if (hopTimer > 12) {
            player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName;
        }
        if (hopTimer > 22) {
            hopTimer = 0;
        }
    } else {
        player.spriteName = player.spriteName == "player_idle_back_hop" ? "player_idle_back" : player.spriteName == "player_idle_hop" ? "player_idle" : player.spriteName;
        hopTimer = 0;
    }

    extern int cameraX, cameraY;
    DrawSprite(player.spriteName, player.x - cameraX, player.y - cameraY, 16, 16);
}

void RenderMap() {
    extern int cameraX, cameraY;
    for (const auto& tile : mapTiles) {
        int drawX = tile.x - cameraX;
        int drawY = tile.y - cameraY;
        switch (tile.type)
        {
        case GROUND:
            DrawSprite("tile_ground", drawX, drawY, tile.width, tile.height);
            break;
        case WALL:
            DrawSprite("tile_wall", drawX, drawY, tile.width, tile.height);
            break;
        case WALL_TOP:
            DrawSprite("tile_wall_top", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_CLOSED:
            DrawSprite("tile_chute_closed", drawX, drawY, tile.width, tile.height);
            break;
        case CHUTE_OPEN:
            DrawSprite("tile_chute_open", drawX, drawY, tile.width, tile.height);
            break;
        default:
            DrawSprite("tile", drawX, drawY, tile.width, tile.height);
            break;
        }
    }
}

void RenderMenu() {
    DrawSprite("menu_splash", 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    DrawSprite("title_sprite", LOGICAL_WIDTH / 2 - 64, 20, 200, 100);
}