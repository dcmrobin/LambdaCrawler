#define SDL_MAIN_HANDLED

#include "HelperFunctions.h"

int LOGICAL_WIDTH = 320;
int LOGICAL_HEIGHT = 240;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
bool is_fullscreen = false;

void setup_renderer() {
    // This call handles scaling, letterboxing, aspect ratio
    SDL_RenderSetLogicalSize(renderer, LOGICAL_WIDTH, LOGICAL_HEIGHT);
    // Optional: choose how to handle aspect ratio
    SDL_RenderSetIntegerScale(renderer, SDL_FALSE); // smooth scaling
}

void toggle_fullscreen() {
    is_fullscreen = !is_fullscreen;
    SDL_SetWindowFullscreen(window, is_fullscreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
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

    // Look for icon file and set it as the application icon
    SDL_Surface* iconSurface = SDL_LoadBMP("assets/sprites/icon.bmp"); 
    if (iconSurface) {
        SDL_SetWindowIcon(window, iconSurface);
        SDL_FreeSurface(iconSurface);
    } else {
        std::cerr << "Warning: Runtime window icon could not be loaded: " << SDL_GetError() << std::endl;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    
    // Set logical size once
    setup_renderer();
    
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
            }
        }

        // Clear only the logical area
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Draw game content
        game_loop();
        
        SDL_RenderPresent(renderer);
    }

    Cleanup();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}