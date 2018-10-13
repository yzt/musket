#include <sdl2/SDL.h>
#include <cstdio>

int main (int argc, char * argv []) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_CreateWindowAndRenderer(1280, 720, 0 /*| SDL_WINDOW_FULLSCREEN*/, &window, &renderer);

    SDL_SetRenderDrawColor(renderer, 42, 0, 0, 255);

    bool should_exit = false;
    SDL_Event ev = {};
    unsigned t0 = SDL_GetTicks();
    unsigned frame_count = 0;
    while (!should_exit) {
        // Process pending events...
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_KEYUP:
                if (SDLK_ESCAPE == ev.key.keysym.sym)
                    should_exit = true;
                break;
            case SDL_QUIT:
                should_exit = true;
                break;
            default:
                break;
            }
        }

        // Do the game loop...
        SDL_RenderClear(renderer);
        SDL_RenderPresent(renderer);

        // FPS counter ...
        frame_count += 1;
        unsigned t1 = SDL_GetTicks();
        if (t1 - t0 >= 1 * 1000) {
            ::printf("Average FPS = %5.1f, average frame time = %5.1f ms\r"
                , double(frame_count) / (t1 - t0) * 1000
                , double(t1 - t0) / frame_count
            );
            t0 = t1;
            frame_count = 0;
        }
    }
    ::printf("\n");

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
