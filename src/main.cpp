#define SDL_MAIN_HANDLED

#include "HelperFunctions.h"

int LOGICAL_WIDTH = 320;
int LOGICAL_HEIGHT = 240;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool is_fullscreen = false;
SDL_Rect viewport = {0, 0, 0, 0};

void update_viewport() {
    int window_width, window_height;
    SDL_GetWindowSize(window, &window_width, &window_height);
    
    // Calculate aspect ratio preserving viewport
    float target_aspect = (float)LOGICAL_WIDTH / LOGICAL_HEIGHT;
    float window_aspect = (float)window_width / window_height;
    int w, h;
    
    if (window_aspect > target_aspect) {
        // Window is wider - bars on sides
        h = window_height;
        w = (int)(h * target_aspect);
    } else {
        // Window is taller - bars on top/bottom
        w = window_width;
        h = (int)(w / target_aspect);
    }
    
    // Center the viewport
    viewport.x = (window_width - w) / 2;
    viewport.y = (window_height - h) / 2;
    viewport.w = w;
    viewport.h = h;
    
    SDL_RenderSetViewport(renderer, &viewport);
}

void toggle_fullscreen() {
    is_fullscreen = !is_fullscreen;
    SDL_SetWindowFullscreen(window, is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
    update_viewport();
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    window = SDL_CreateWindow(
        "Lambda Crawler",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        LOGICAL_WIDTH * 2, LOGICAL_HEIGHT * 2,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set blend mode for transparency support
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Set initial viewport
    update_viewport();
    
    Init(renderer);
    game_setup();

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
                    if (event.window.event == SDL_WINDOWEVENT_RESIZED ||
                        event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                        update_viewport();
                    }
                    break;
            }
        }

        // Clear the entire window to black
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Draw game content within the viewport
        game_loop();
        
        SDL_RenderPresent(renderer);
    }

    Cleanup();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}