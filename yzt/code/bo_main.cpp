#include <sdl2/SDL.h>
#include <cmath>
#include <cstdio>

using byte = unsigned char;

#include "bo_math.hpp"
#include "bo_render.hpp"

struct Config {
    int target_fps = 120;
    int window_width = 600;
    int window_height = 0;
    float window_aspect_ratio = 3.0f / 4.0f;

    float paddle_speed = 1000.0f;
    float paddle_vert_pos = 0.95f;
    Vec2f paddle_dims = {160, 20};
    float ball_radius = 10.0f;
    float ball_speed = 2000.0f;
};

struct Input {
    float movement = 0.0f; // in [-1..1]
    bool exit = false;
    bool action = false;

    bool left_pressed = false;
    bool right_pressed = false;
};

struct State {
    Point2f paddle_pos = {0, 300};
    Point2f ball_pos = {};
    Vec2f ball_dir = {};
    bool ball_in_movement = false;
};

int main (int argc, char * argv []) {
    Config config;
    Input input;
    State state;

    config.window_height = Round(config.window_width / config.window_aspect_ratio);

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window * window = nullptr;
    SDL_Renderer * renderer = nullptr;
    SDL_CreateWindowAndRenderer(config.window_width, config.window_height, 0 /*| SDL_WINDOW_FULLSCREEN*/, &window, &renderer);
    SDL_GetWindowSize(window, &config.window_width, &config.window_height);
    SDL_SetWindowTitle(window, "BrykOut");

    SDL_Texture * tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, config.window_width, config.window_height);
    SDL_assert(tex);

    state.paddle_pos.y = config.paddle_vert_pos * config.window_height;

    double target_frame_time_s = 1.0 / config.target_fps;
    double const inv_pfc_freq = 1.0 / SDL_GetPerformanceFrequency();
    double next_frame_start_s = inv_pfc_freq * SDL_GetPerformanceCounter() + target_frame_time_s;
    double wastage = 0.0;

    SDL_Event ev = {};
    unsigned t0 = SDL_GetTicks();
    unsigned frame_count = 0;
    for (;;) {
        // Process pending events...
        input.action = false;
        input.exit = false;
        while (SDL_PollEvent(&ev)) {
            switch (ev.type) {
            case SDL_KEYDOWN:
                switch (ev.key.keysym.sym) {
                case SDLK_a: case SDLK_LEFT: input.left_pressed = true; break;
                case SDLK_d: case SDLK_RIGHT: input.right_pressed = true; break;
                case SDLK_SPACE: input.action = true;
                }
                break;
            case SDL_KEYUP:
                switch (ev.key.keysym.sym) {
                case SDLK_a: case SDLK_LEFT: input.left_pressed = false; break;
                case SDLK_d: case SDLK_RIGHT: input.right_pressed = false; break;
                case SDLK_ESCAPE: input.exit = true; break;
                }
                break;
            case SDL_QUIT:
                input.exit = true;
                break;
            }
        }

        // Process the input...
        if (input.exit)
            break;

        if (input.left_pressed && !input.right_pressed)
            input.movement = -1.0f;
        else if (input.right_pressed && !input.left_pressed)
            input.movement = 1.0f;
        else
            input.movement = 0.0f;

        if (input.action && !state.ball_in_movement) {
            state.ball_in_movement = true;
            state.ball_dir = Normalize({(input.movement >= 0 ? 1.0f : -1.0f), -1.0f});
        }

        // Do the update...
        state.paddle_pos.x += input.movement * config.paddle_speed * float(target_frame_time_s);
        
        if (!state.ball_in_movement) {
            state.ball_pos = {
                state.paddle_pos.x + config.paddle_dims.x / 2,
                state.paddle_pos.y - config.ball_radius
            };
        } else {
            auto bp = state.ball_pos + state.ball_dir * (config.ball_speed * float(target_frame_time_s));

            if (bp.x < 0)
                if (state.ball_dir.x < 0)
                    state.ball_dir.x = -state.ball_dir.x;
            if (bp.x > config.window_width)
                if (state.ball_dir.x > 0)
                    state.ball_dir.x = -state.ball_dir.x;
            if (bp.y < 0)
                if (state.ball_dir.y < 0)
                    state.ball_dir.y = -state.ball_dir.y;
            if (bp.y > config.window_height)
                if (state.ball_dir.y > 0)
                    state.ball_dir.y = -state.ball_dir.y;

            state.ball_pos = bp;
        }
        
        // Do the render...
        Canvas canvas = {};
        SDL_LockTexture(tex, nullptr, &canvas.pixels_raw, &canvas.pitch_bytes);
        canvas.width = config.window_width;
        canvas.height = config.window_height;

        Render_Clear(&canvas, {0, 0, 0});

        Render_AAB(
            &canvas,
            Round(state.paddle_pos.x), Round(state.paddle_pos.y),
            Round(config.paddle_dims.x), Round(config.paddle_dims.y),
            {255, 0, 0}
        );

        Render_Circle(
            &canvas,
            Round(state.ball_pos.x), Round(state.ball_pos.y),
            Round(config.ball_radius),
            {0, 255, 0}
        );

        SDL_UnlockTexture(tex);

        //SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, tex, nullptr, nullptr);
        SDL_RenderPresent(renderer);

        // FPS counter ...
        frame_count += 1;
        unsigned t1 = SDL_GetTicks();
        if (t1 - t0 >= 1 * 1000) {
            ::printf("FPS = %0.2f, frame time = %0.2fms, wastage = %0.2fms (%0.1f%%)           \r"
                , double(frame_count) / (t1 - t0) * 1000
                , double(t1 - t0) / frame_count
                , 1000 * wastage / frame_count
                , (1000 * wastage) / double(t1 - t0) * 100
            );
            t0 = t1;
            frame_count = 0;
            wastage = 0;
        }

        // Waste the rest of the frame time...
        double now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        double waste_start = now_s;
        while (now_s < next_frame_start_s) {
            SDL_Delay(0);
            now_s = inv_pfc_freq * SDL_GetPerformanceCounter();
        }
        next_frame_start_s += target_frame_time_s;
        wastage += now_s - waste_start;
    }
    ::printf("\n");

    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
