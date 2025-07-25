#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <iostream>
#include "game.h"
#include "Sprites.h"

// Constants and globals
const int LOGICAL_WIDTH = 320;    // Game's native resolution (4:3)
const int LOGICAL_HEIGHT = 240;
SDL_Window* window = nullptr;
bool is_fullscreen = false;
SDL_Rect viewport = {0, 0, 0, 0};  // Scaled viewport

// Toggle fullscreen
void toggle_fullscreen() {
    is_fullscreen = !is_fullscreen;
    SDL_SetWindowFullscreen(window, is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    SDL_GetWindowSize(window, &viewport.w, &viewport.h);  // Update viewport immediately
}

// Update viewport to maintain aspect ratio
void update_viewport() {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);

    float game_aspect = (float)LOGICAL_WIDTH / LOGICAL_HEIGHT;
    float window_aspect = (float)window_width / window_height;

    if (game_aspect > window_aspect) {
        // Window is taller (pillarbox)
        viewport.w = window_width;
        viewport.h = (int)(window_width / game_aspect);
        viewport.x = 0;
        viewport.y = (window_height - viewport.h) / 2;
    } else {
        // Window is wider (letterbox)
        viewport.h = window_height;
        viewport.w = (int)(window_height * game_aspect);
        viewport.x = (window_width - viewport.w) / 2;
        viewport.y = 0;
    }

    SDL_RenderSetViewport(renderer, &viewport);
}

int main(int argc, char* argv[]) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Create window
    window = SDL_CreateWindow(
        "Lambda Crawler",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LOGICAL_WIDTH, LOGICAL_HEIGHT,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create renderer (with vsync)
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize viewport and sprite system
    update_viewport();
    Init(renderer);
    game_setup();

    // Main loop
    bool running = true;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_F11) {
                        toggle_fullscreen();
                    }
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                        update_viewport();
                    }
                    break;
            }
        }

        // Clear screen (black bars + dark gray game area)
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Black bars
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);  // Game background
        SDL_RenderFillRect(renderer, &viewport);

        // Game rendering (uses LOGICAL_WIDTH/HEIGHT coordinates)
        game_loop();

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    Cleanup();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}